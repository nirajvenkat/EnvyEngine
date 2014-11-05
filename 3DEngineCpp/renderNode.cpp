// renderNode.cpp
//
// The Master Controller-side representation of a hardware rendering node. The hardware rendering node is responsible
// for rendering specific requests made by the master controller.

#include <stdlib.h>
#include "renderNode.h"
#include "renderTask.h"
#include "time.h"

RenderNode::RenderNode(int number) 
{
	mNumber = number;
	mLastLatency = -1; // Latency uninitialized until first response received.
	mLatencies = (float*)malloc(sizeof(float)*NODE_LATENCY_WINDOW);
}

RenderNode::~RenderNode() {
	free(mLatencies);
}

void RenderNode::assignTask(class RenderTask *task) {

	// Clear current task
	clearTask();
	mCurrentTask = task;

	// Set task start time
	mLastAssignTime = Time::GetTime();

	// TODO: Send task payload over the network
}

void RenderNode::receiveResponse() {
	// TODO: Receive response from the network
	// This will be called by a callback (or similar) for when a response is received from a hardware node on the network.

	// Update response time
	mLastLatency = Time::GetTime() - mLastAssignTime;
}

void RenderNode::clearTask() {
	delete(mCurrentTask);
}

int RenderNode::getNumber() {
	return mNumber;
}

void refreshRates();