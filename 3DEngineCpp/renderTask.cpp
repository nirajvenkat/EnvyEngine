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
	this->mFramePixels = NULL;
	this->mBitmap = NULL;
	this->seqNo = seqNo;
	this->mTimeStamp = timeStamp;
	this->mSliceIdx = 0;
}

RenderTask::~RenderTask() {
	if (mFramePixels != NULL)
		free(mFramePixels);
	if (mBitmap != NULL)
		delete mBitmap;
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

void RenderTask::setResultBitmap(Gdiplus::Bitmap *bitmap, void *pixels) {
	mBitmap = bitmap;
	mFramePixels = pixels;
}

void RenderTask::getResultBitmap(Gdiplus::Bitmap **bitmap, void **pixels) {
	*bitmap = mBitmap;
	*pixels = mFramePixels;
}

void RenderTask::setDimensions(int w, int h) {
	mWidth = w;
	mHeight = h;
}

int RenderTask::getWidth() {
	return mWidth;
}

int RenderTask::getHeight() {
	return mHeight;
}

unsigned long RenderTask::getSeqNo() {
	return seqNo;
}
