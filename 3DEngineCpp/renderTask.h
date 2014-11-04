// renderTask.h
//
// RenderTask tells the renderer what to render. The task payload is sent over the network to individual hardware nodes, which render frames
// according to the parameters in the task payload. Render tasks are also used to direct nodes to do things like load world data and adjust overall
// rendering parameters. The taskType attribute determines what kind of task is being sent.

#ifndef RENDERTASK_H
#define RENDERTASK_H

#include "renderTaskPayload.h"

class RenderTask {

public:

	RenderTask(unsigned long seqNo);
	virtual ~RenderTask();

	unsigned long getSeqNo();
	RenderTaskPayload *getPayload();
	void setPayload(int taskType, size_t payloadSize, void *payloadData);
	void clearPayload();

private:
	unsigned long seqNo;		// Task sequence number
	RenderTaskPayload *payload; // Actual task payload to be sent over the net.
};

#endif