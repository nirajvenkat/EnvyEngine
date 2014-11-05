// renderTask.cpp
//
// RenderTask tells the renderer what to render. The task payload is sent over the network to individual hardware nodes, which render frames
// according to the parameters in the task payload. Render tasks are also used to direct nodes to do things like load world data and adjust overall
// rendering parameters. The taskType attribute determines what kind of task is being sent.

#include "renderTask.h"
#include <string.h>
#include <stdlib.h>

RenderTask::RenderTask(unsigned long seqNo) {
	this->seqNo = seqNo;
}

RenderTask::~RenderTask() {
	clearPayload();
}

void RenderTask::setPayload(int taskType, size_t payloadDataSize, void *payloadData) {
	RenderTaskPayload *payload;

	payload = (RenderTaskPayload*)malloc(RENDERTASKPAYLOAD_HEADER_SIZE + payloadDataSize);
	if (payload) {
		memset(payload, 0, sizeof(payload));
		payload->type = taskType;
		payload->totalSize = RENDERTASKPAYLOAD_HEADER_SIZE + payloadDataSize;
		payload->dataSize = payloadDataSize;
		memcpy(&payload->taskData, payloadData, payloadDataSize); // Load actual task data

		mPayload = payload;
	}
}

RenderTaskPayload *RenderTask::getPayload() {
	return this->mPayload;
}

void RenderTask::clearPayload() {
	if (mPayload != NULL) {
		free(mPayload);
		mPayload = NULL;
	}
}
