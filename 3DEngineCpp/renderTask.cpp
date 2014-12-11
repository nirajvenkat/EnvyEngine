// renderTask.cpp
//
// RenderTask tells the renderer what to render. The task payload is sent over the network to individual hardware nodes, which render frames
// according to the parameters in the task payload. Render tasks are also used to direct nodes to do things like load world data and adjust overall
// rendering parameters. The taskType attribute determines what kind of task is being sent.
//
// Note: Class is being repurposed, comment may be out of date.

#include "renderTask.h"
#include <string.h>
#include <stdlib.h>

RenderTask::RenderTask(unsigned long seqNo, double timeStamp) {
	this->mPayload = NULL;
	this->seqNo = seqNo;
	this->mTimeStamp = timeStamp;
	this->mSliceIdx = 0;
}

RenderTask::~RenderTask() {
	clearPayload();
}

void RenderTask::setProjectionMatrix(Matrix4f & matrix) {
	mProjectionMatrix = matrix;
}

Matrix4f *RenderTask::getProjectionMatrix() {
	return &mProjectionMatrix;
}

int RenderTask::getSlices() {
	return mSlices;
}

void RenderTask::setSliceIdx(int idx, int slices) {
	mSliceIdx = idx;
	mSlices = slices;
}

int RenderTask::getSliceIndex() {
	return mSliceIdx;
}

double RenderTask::getTimeStamp() {
	return mTimeStamp;
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

unsigned long RenderTask::getSeqNo() {
	return seqNo;
}
