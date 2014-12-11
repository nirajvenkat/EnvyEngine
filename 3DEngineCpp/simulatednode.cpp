// simulatednode.cpp
//
// For testing the load balancing features of MasterController and RenderNode without the network.

#include "simulatednode.h"
#include "renderTask.h"
#include "renderNode.h"
#include "framedriver.h"
#include <SDL2/SDL_thread.h>
#include <map>
#include <cstdlib>
#include <ctime>
#include <map>

extern FrameDriver *gFrameDriver;
std::map<RenderNode *,RenderTask*> gTasks;

// Statics
int SimulatedNode::bitmapWidth;
int SimulatedNode::bitmapHeight;
std::map<unsigned int, char*> SimulatedNode::framePool;

// Callbacks
Uint32 taskTimeout(Uint32 interval, void *vself);

SimulatedNode::SimulatedNode(RenderNode *parent, int minLatency, int maxLatency, int dropTime) {
	mMinLatency = minLatency;
	mMaxLatency = maxLatency;
	mDropTime = dropTime;
	mParent = parent;
	mTasksDone = 0;

	// Seed random number generator
	srand(static_cast <unsigned> (time(0)));
}

void SimulatedNode::initSimNodes(int width, int height) {
	bitmapWidth = width;
	bitmapHeight = height;
	framePool.clear();
}

void SimulatedNode::acceptTask(RenderTask *rt) {

	SDL_TimerID tid;
	Uint32 waitTime;

	gTasks[this->mParent] = rt;

	// Accept render task
	// fprintf(stderr, "SimNode %d accepted render task\n", mParent->getNodeId());

	// Randomly generate a rough wait time in ms
	if (mMaxLatency > mMinLatency)
		waitTime = mMinLatency + rand() / (RAND_MAX / (mMaxLatency - mMinLatency));
	else
		waitTime = mMinLatency;

	if (mTasksDone > mDropTime && mDropTime > 0) {
		mMinLatency = 500;
		mMaxLatency = 550;
	}

	// Start timer
	mTid = SDL_AddTimer(waitTime, taskTimeout, (void*)this);
}

void SimulatedNode::finishTask() {
	SDL_RemoveTimer(mTid); // Wait time over.
	mParent->receiveResponse();
	
	mTasksDone++;
}

int SimulatedNode::getId() {
	return mParentId;
}

// Threading utility functions
void SimulatedNode::lock() {
	SDL_LockMutex(mTCrit);
}

void SimulatedNode::unlock() {
	SDL_UnlockMutex(mTCrit);
}

// Callbacks
Uint32 taskTimeout(Uint32 interval, void *vself) { // This executes in its own thread, so be careful to ensure all ops are threadsafe.
	SimulatedNode *self = (SimulatedNode*)vself;
	self->finishTask();
	return interval;
}