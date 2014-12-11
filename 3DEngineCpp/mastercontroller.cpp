// mastercontroller.cpp
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#include "mastercontroller.h"
#include "renderer.h"
#include "renderNode.h"
#include "renderTask.h"
#include "windows.h"
#include <objidl.h>
#include <gdiplus.h>
#include <GdiPlusImageCodec.h>
#include "framedriver.h"
#include "simnodetests.h"
#include "overlay.h"
#include <queue>
#include <stack>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <assert.h>
#include "time.h"
#include "game.h"
#include "camera.h"
#include "envy_mc_driver.h"

using namespace std;
// TODO: Network code to wait for incoming real hardware node connections.

#ifdef TEST_MC
std::map<unsigned int, class RenderNode*> *gNodes;
Renderer *gRenderer;
extern CoreEngine *gEngine;
#endif
extern Camera *gCamera;

MasterController::MasterController(int frameRateMax, Game *game) {

	mGame = game;
	mFrameTime = Time::GetTime();
	mFrameRateMax = frameRateMax;
	mMaxNodeId = 0;
	mStartSem = NULL;
	mNodeTimeshare = NULL;
	mNodes.clear();
	mFramePeriod = 1.0 / (double)frameRateMax; // Default to max frame period
	mMinFramePeriod = mFramePeriod;
	mLastTaskId = 0;

#ifdef TEST_MC
	gNodes = &this->mNodes;
#endif
}

// Destroy the master controller in a thread-safe manner.
MasterController::~MasterController() {

	SDL_LockMutex(mTCrit);

	if (mInitialized) {
		SDL_DestroySemaphore(mStartSem);
	}

	SDL_DestroyMutex(mTCrit); // DESTROY
}

// Entry point for a new MC thread.
int MasterController::_startMCThread(void* mc) {
	((MasterController*)mc)->_execute();
	return 0;
}

// Set up thread, put it in a waiting state.
void MasterController::init(int width, int height) {

	// Create critical section
	mTCrit = SDL_CreateMutex();
	if (mTCrit == NULL) {
		fprintf(stderr, "Could not create SDL_Mutex: %s\n", SDL_GetError());
		return;
	}

	// Create the start semaphore
	mStartSem = SDL_CreateSemaphore(0);
	if (mStartSem == NULL) {
		fprintf(stderr, "Could not create SDL_Sem: %s\n", SDL_GetError());
		return;
	}

	// Create the mastercontroller thread.
	/*
	SDL_Thread *mThread = SDL_CreateThread(MasterController::_startMCThread, "Envy MasterController", this);
	if (mThread == NULL) {
		fprintf(stderr, "SDL_CreateThread failed: %s\n", SDL_GetError());
		return;
	}*/

	// Create renderer
	mRenderer = new Renderer(mGame);
	mRenderer->initOutputWindow(width, height, "EnvyEngine Master Controller");
#ifdef TEST_MC
	gRenderer = mRenderer;
	mRenderer->setCoreEngine(gEngine);
#endif

	// Initialize GdiPlus
	Gdiplus::GdiplusStartupInput startupInput;
	ULONG_PTR token;
	GdiplusStartup(&token, &startupInput, NULL);

	// Start server
	enableRegistration();

	// Init successful
	fprintf(stderr, "Master Controller Initialized...\n");
	mInitialized = true;

	_execute();
}

// Kick off the thread - may not be necessary
void MasterController::run() {
	fprintf(stderr, "Master Controller Executing...\n");
	SDL_SemPost(mStartSem);
}

// Do not call this externally (OK, we are calling it externally now since we have seperate modes)
void MasterController::_execute() {

	unsigned long last = 0;
	static unsigned long tempTime = 0;
	int pause = 1000 / mFrameRateMax;
	int statTimer = 0;
	Frame *curFrame;
	std::stack<unsigned int> markedNodes;

	SDL_SemWait(mStartSem);
	fprintf(stderr, "Master controller out of wait state.\n");

#ifdef SIMULATE
	fprintf(stderr, "Loading node simulation.\n");
		RunSimNodeTest(SIMULATE, this);
#endif

	// Inner loop
	while (true) {
		
		std::map<unsigned int, RenderNode*>::iterator it;
		double curTime = Time::GetTime();
		
		lock();

		// *** Work on frames ***

		// This is the portion of the master controller that checks to see if all slices are present, then if they are,
		// renders them *all* to the current viewport. The overlay is triggered later on, if overlay mode is on.
		// assert(mFrameQueue.size() <= mNodes.size());
		
		// FRAME RENDERING
		if (mFrameQueue.size() == mNodes.size()      // We have N ready tasks.
			&& curTime - mFrameTime >= mFramePeriod  // Correct period has elapsed.
			) {
			while (mFrameQueue.size()) {
				Frame *curFrame = mFrameQueue.top();
				mRenderer->renderFrame(curFrame);
				mFrameQueue.pop();
				delete curFrame; // Fixed memory leak here.
			}
			mRenderer->commit();
			// Update the wait time so that we know how long the whole operation took.
			//mWaitPeriod = Time::GetTime() - mFrameTime;
		}

		// TASK CREATION
		if (mFrameQueue.size() == 0 &&
			mWaitingTasks.size() == 0) { // We have nothing to do. Move to a new frame.

			// Set a new frame period. TODO: Implement moving average here for signal smoothing.
			if (mWaitPeriod > mMinFramePeriod)
				mFramePeriod = mWaitPeriod;
			else
				mFramePeriod = mMinFramePeriod;
			
			mFrameTime = curTime; // Update frametime

			// Create a new set of tasks
			for (int i = 0; i < mNodes.size(); i++) {
				RenderTask *curTask = new RenderTask(mLastTaskId++, mFrameTime);

				// Set the projection matrix from the Camera position on the master controller, NOT the nodes
				// TODO *** curTask->setProjectionMatrix(matrix);

				curTask->setSliceIdx(i, mNodes.size());
				mWaitingTasks.insert(curTask);
			}
		}

		// Render based on node state.
		if (!mWaitingTasks.empty()) {
			// iterate over nodes to assign tasks
			for (it = mNodes.begin(); it != mNodes.end() && !mWaitingTasks.empty(); ++it) {
				RenderNode *cn = it->second;
				if (cn->getStatus() == RenderNode::READY) {
					assignTask(cn, nextWaitingTask());
				}
			}
		}
		for (it = mNodes.begin(); it != mNodes.end(); ++it) {
			RenderNode *cn = it->second;
			if (cn->getStatus() == RenderNode::RECEIVED_DATA) {
				mFrameQueue.push(cn->unloadFinishedFrame()); // Brings the node to READY status
				mNodeTaskMap[cn->getNodeId()] = NULL;
			}
		}


		// *** Work on nodes ***

		// Gather problem nodes
		for (it = mNodes.begin(); it != mNodes.end(); ++it) { // iterate over nodes
			RenderNode *cn = it->second;
			if (cn->getAvgLatency() > MC_MAX_LATENCY) {
				fprintf(stderr, "WARNING: Node %d exceeded max latency. Dropping node.\n");
				markedNodes.push(it->first);
			}
		}

		// Drop all marked nodes.
		unsigned int markedId;
		while (markedNodes.size() > 0) {
			dropNode(markedNodes.top());
			markedNodes.pop();
		}

		// Assign tasks - Simple round-robin (may move to RSS later)
		/* SUPERCEDED
		for (it = mNodes.begin(); it != mNodes.end(); ++it) { // iterate over nodes
			RenderNode *cn = it->second;
			switch (cn->getStatus()) {
				case RenderNode::READY:
					cn->assignTask(NULL);
				break;
				case RenderNode::RECEIVED_DATA:
					mFrameQueue.push(cn->unloadFinishedFrame());
				break;
			}
		}*/

		// Render available finished frames
		/* SUPERCEDED
		if (!mFrameQueue.empty())
		{
			curFrame = mFrameQueue.top();
			mFrameQueue.pop();
			//fprintf(stderr, "MasterController: Rendering frame %d\n", curFrame->getModelTime());
			mRenderer->renderFrame(curFrame);
			delete(curFrame);
		}*/

		unlock();

		statTimer++;
		if (statTimer > 30) {
			statTimer = 0;
			refreshNodeTimeshares();
			debugNodeStatistics();
		}

		Sleep(pause);
	}
}
// Add a frame to the frame queue (thread-safe).
void MasterController::addFrame(Frame *newFrame) {
	this->lock();
	mFrameQueue.push(newFrame);
	this->unlock();
}

// Add a render node
void MasterController::addNode(RenderNode *rn) {

	unsigned int nodeId = rn->getNodeId();

	mNodes[nodeId] = rn;

	// Add it to the renderer's overlay
	mRenderer->addNodeToOverlay(nodeId);

	// Reset timeshare array
	if (mNodeTimeshare)
		delete[] mNodeTimeshare;
	if (!mNodes.empty())
		mNodeTimeshare = new float[mNodes.size()];
	else mNodeTimeshare = NULL;
}

void MasterController::dropNode(unsigned int nodeId) {

	lock();

	RenderTask *curTask;
	RenderNode *rn = mNodes[nodeId];
	mNodes.erase(nodeId);

	// Unassign task from node
	/*
	curTask = mNodeTaskMap[nodeId];
	if (curTask != NULL)
		mWorkingTasks.erase(curTask);
	mNodeTaskMap[nodeId] = NULL;

	// Put the task back in the waitlist.
	mWaitingTasks.insert(curTask);*/

	delete(rn);

	// Remove it from the Renderer's overlay
	mRenderer->removeNodeFromOverlay(nodeId);

	// Reset timeshare array
	if (mNodeTimeshare)
		delete[] mNodeTimeshare;
	if (!mNodes.empty())
		mNodeTimeshare = new float[mNodes.size()];
	else mNodeTimeshare = NULL;

	unlock();
}

// Referesh timeshares for each node based on performance.
// Each entry in mNodeTimeshare will gain a timeshare percentage.
void MasterController::refreshNodeTimeshares() {
	
	int i, n;
	RenderNode *cn;
	float invLatency = 0.0f;

	// Thread safe
	lock();

	n = mNodes.size();
	std::map<unsigned int, RenderNode*>::iterator it;
	i = 0;
	for (it = mNodes.begin(); it != mNodes.end(); ++it) {
		cn = it->second;
		invLatency += 1.0f / cn->getAvgLatency();
	}
	invLatency = 1 / invLatency;
	for (it = mNodes.begin(); it != mNodes.end(); ++it) {
		cn = it->second;
		mNodeTimeshare[i++] = invLatency * (1.0f / cn->getAvgLatency());
	}
	unlock();
}

// Threading utility functions
void MasterController::lock() {
	SDL_LockMutex(mTCrit);
}

void MasterController::unlock() {
	SDL_UnlockMutex(mTCrit);
}

// Debug functions
void MasterController::debugNodeStatistics() {
	std::map<unsigned int, RenderNode*>::iterator it;
	char latencyText[256];
	fprintf(stderr, "Node Statistics: Latency( ");
	int i = 0;
	for (it = mNodes.begin(); it != mNodes.end(); ++it) { // iterate over nodes
		RenderNode *curNode = it->second;
		sprintf(latencyText, "Node %d Latency (%.1fms)",
			it->first,
			curNode->getAvgLatency());
		mRenderer->updateNodeOnOverlay(it->first, latencyText, mNodeTimeshare[i++]);
		fprintf(stderr, "%.1fms ", curNode->getAvgLatency());
	}
	fprintf(stderr, ") Timeshares( ");
	i = 0;
	for (it = mNodes.begin(); it != mNodes.end(); ++it) { // iterate over nodes
		unsigned int id = it->first;
		fprintf(stderr, "%.2f%% ", mNodeTimeshare[i++]*100.0f);
	}
	fprintf(stderr, ")\n");
}

// Assign a task to a node
RenderTask *MasterController::nextWaitingTask() {
	RenderTask *task = NULL;
	if (!mWaitingTasks.empty())
		task = *mWaitingTasks.begin();
	return task;
}

void MasterController::assignTask(RenderNode *node, RenderTask *task) {
	
	node->assignTask(task);

	lock();
	mWaitingTasks.erase(task);
	mWorkingTasks.insert(task);
	unlock();
}

void MasterController::unassignTask(RenderNode *node, RenderTask *task) {
	
	lock();
	mWorkingTasks.erase(task);
	mNodeTaskMap[node->getNodeId()] = NULL;
	unlock();

	// Put the task back in the waitlist.
	mWaitingTasks.insert(task);
}

void MasterController::resetWork() {

	lock();

	// Clear current task lists
	clearWorkingTasks();
	clearWaitingTasks();

	// Dump frame queue. In the event of node addition or deletion, bitmap
	// sizes will change.


	unlock();
}

// Functions to clear tasks and/or frames
void MasterController::clearWorkingTasks() {
	mNodeTaskMap.clear();
	for (std::set<RenderTask*>::iterator it = mWorkingTasks.begin(); 
		                                 it != mWorkingTasks.end(); it++)
	{
		delete *it;
	}
	mWorkingTasks.clear();
}

void MasterController::clearWaitingTasks() {
	for (std::set<RenderTask*>::iterator it = mWaitingTasks.begin();
		it != mWaitingTasks.end(); it++)
	{
		delete *it;
	}
	mWaitingTasks.clear();
}

void MasterController::clearFrames() {
	while (!mFrameQueue.empty()) {
		Frame *f = mFrameQueue.top();
		delete f;
		mFrameQueue.pop();
	}
}