// mastercontroller.cpp
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#include "mastercontroller.h"
#include "renderer.h"
#include "renderNode.h"
#include "windows.h"
#include "framedriver.h"
#include "simnodetests.h"
#include <queue>
#include <stack>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

using namespace std;

// TODO: Network code to wait for incoming real hardware node connections.

MasterController::MasterController(int frameRateMax) {
	mFrameRateMax = frameRateMax;
	mMaxNodeId = 0;
	mStartSem = NULL;
	mNodeTimeshare = NULL;
	mNodes.clear();
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
void MasterController::init() {

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
	SDL_Thread *mThread = SDL_CreateThread(MasterController::_startMCThread, "Envy MasterController", this);
	if (mThread == NULL) {
		fprintf(stderr, "SDL_CreateThread failed: %s\n", SDL_GetError());
		return;
	}

	// Create renderer
	mRenderer = new Renderer();
	mRenderer->initOutputWindow(640, 480, "Envy Master Controller");

	// Init successful
	fprintf(stderr, "Master Controller Initialized...\n");
	mInitialized = true;
}

// Kick off the thread.
void MasterController::run() {
	fprintf(stderr, "Master Controller Executing...\n");
	SDL_SemPost(mStartSem);
}

// Do not call this externally
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
		
		lock();

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
		}

		// Render available finished frames
		if (!mFrameQueue.empty())
		{
			curFrame = mFrameQueue.top();
			mFrameQueue.pop();
			//fprintf(stderr, "MasterController: Rendering frame %d\n", curFrame->getModelTime());
			mRenderer->renderFrame(curFrame);
			delete(curFrame);
		}

		/*
		if (gFrameDriver->hasFrames()) {

			curFrame = gFrameDriver->nextFrame();
			curFrame->setSurface(curFrame->getSurface());
			mRenderer->renderFrame(curFrame);
			delete(curFrame);
		}
		*/
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

	// Reset timeshare array
	if (mNodeTimeshare)
		delete[] mNodeTimeshare;
	if (!mNodes.empty())
		mNodeTimeshare = new float[mNodes.size()];
	else mNodeTimeshare = NULL;
}

void MasterController::dropNode(unsigned int nodeId) {

	lock();

	RenderNode *rn = mNodes[nodeId];
	mNodes.erase(nodeId);
	delete(rn);

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
	fprintf(stderr, "Node Statistics: Latency( ");
	for (it = mNodes.begin(); it != mNodes.end(); ++it) { // iterate over nodes
		RenderNode *curNode = it->second;
		fprintf(stderr, "%.1fms ", curNode->getAvgLatency());
	}
	fprintf(stderr, ") Timeshares( ");
	int i = 0;
	for (it = mNodes.begin(); it != mNodes.end(); ++it) { // iterate over nodes
		unsigned int id = it->first;
		fprintf(stderr, "%.2f%% ", mNodeTimeshare[i++]*100.0f);
	}
	fprintf(stderr, ")\n");
}