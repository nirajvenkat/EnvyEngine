// renderNode.cpp
//
// The Master Controller-side representation of a hardware rendering node. The hardware rendering node is responsible
// for rendering specific requests made by the master controller.

#include "envy_network.h"
#include <stdlib.h>
#include "renderNode.h"
#include "renderTask.h"
#include "frame.h"
#include "simulatednode.h"
#include "time.h"

RenderNode::RenderNode(unsigned int number) 
{
	mId = number;
	mCurSample = 0;
	mLastLatency = 1;
	mCommandRate = 1;
	mResponseRate = 1;
	mLatencySamples = (float*)malloc(sizeof(float)*NODE_LATENCY_WINDOW);
	memset(mLatencySamples, 0, sizeof(float)*NODE_LATENCY_WINDOW);
	mStatus = Status::READY;
#ifdef SIMULATE
	mSimNode = NULL;
#endif
	mCurrentTask = NULL;
	mAddr = (in_addr*)malloc(sizeof(in_addr));
}

RenderNode::~RenderNode() {
	free(mLatencySamples);
	free(mAddr);
#ifdef SIMULATE
	if (mSimNode)
		delete(mSimNode);
#endif
}

#ifdef SIMULATE // Simulated nodes
void RenderNode::setupSimNode(int minLatency, int maxLatency, int dropTime)
{
	mSimNode = new SimulatedNode(this, minLatency, maxLatency, dropTime);
}

/*
void RenderNode::receiveSimData(Frame *newFrame) {
	mFinishedFrame = newFrame;
}*/

void RenderNode::simNullResponse() {
	updateResponseTime();
	mStatus = Status::READY;
}
#endif

void RenderNode::assignTask(class RenderTask *task) {

	mStatus = Status::BUSY;

	// Clear current task
	clearTask();
	mCurrentTask = task;

	// Set task start time
	mLastAssignTime = Time::GetTime();
	
#ifdef SIMULATE
	mSimNode->acceptTask(task);
#endif

	// TODO: Send task payload over the network
}

void RenderNode::receiveResponse() {
	// TODO: Receive response from the network
	// This will be called by a callback (or similar) for when a response is received from a hardware node on the network.
	updateResponseTime(); // Update average response time.
	//mStatus = Status::RECEIVED_DATA;
	mStatus = Status::LOADING_DATA;
}

void RenderNode::clearTask() {
	if (mCurrentTask) {
		delete(mCurrentTask);
		mCurrentTask = NULL;
	}
}

int RenderNode::getNodeId() {
	return mId;
}

// Temporary
void RenderNode::setFinishedFrame(Frame *f) {
	mFinishedFrame = f;
	mStatus = RECEIVED_DATA;
}

Frame *RenderNode::unloadFinishedFrame()
{
	Frame *ff = mFinishedFrame;
	mFinishedFrame = NULL;

	clearTask();

	mStatus = Status::READY;

	return ff;
}

RenderNode::Status RenderNode::getStatus() {
	return mStatus;
}

float RenderNode::getAvgLatency() {
	return mResponseRate;
}

// Computes a moving average of response times with window size equal to NODE_LATENCY_WINDOW.
// Change NODE_LATENCY_WINDOW to a larger value for added smoothness.
void RenderNode::updateResponseTime() {
	// Update response time
	mLastLatency = (float)(Time::GetTime() - mLastAssignTime) * 1000;
	mLatencySamples[mCurSample++] = mLastLatency;

	// Point to sample slated for removal.
	if (mCurSample >= NODE_LATENCY_WINDOW)
		mCurSample = 0;

	// Recompute moving average
	mResponseRate += mLastLatency / (float)NODE_LATENCY_WINDOW - mLatencySamples[mCurSample] / (float)NODE_LATENCY_WINDOW;
}

void RenderNode::setNodeInAddr(in_addr *addr) {
	memcpy(mAddr, addr, sizeof(in_addr));
}

void RenderNode::getNodeInAddr(in_addr *dest) {
	memcpy(dest, mAddr, sizeof(in_addr));
}