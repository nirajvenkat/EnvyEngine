// renderNode.cpp
//
// The Master Controller-side representation of a hardware rendering node. The hardware rendering node is responsible
// for rendering specific requests made by the master controller.

#include "renderNode.h"
#include "renderTask.h"

#include "time.h"

RenderNode::RenderNode(int number) 
{
	this->number = number;
	this->lastLatency = -1; // Latency uninitialized until first response received.
}

RenderNode::~RenderNode() {}

void RenderNode::assignTask(class RenderTask *task) {

	// Clear current task
	clearTask();

	currentTask = task;

	// TODO: Send task payload over the network


}

void RenderNode::receiveResponse() {
	// TODO: Receive response from the network
	// This will be called by a callback (or similar) for when a response is received from a hardware node on the network.

	// Update response time
	lastLatency = Time::GetTime() - lastAssignTime;
}

void RenderNode::clearTask() {
	delete(currentTask);
}

int getNumber();
void refreshRates();