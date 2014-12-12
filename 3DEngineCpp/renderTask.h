// renderTask.h
//
// RenderTask tells the renderer what to render. The task payload is sent over the network to individual hardware nodes, which render frames
// according to the parameters in the task payload. Render tasks are also used to direct nodes to do things like load world data and adjust overall
// rendering parameters. The taskType attribute determines what kind of task is being sent.

#ifndef RENDERTASK_H
#define RENDERTASK_H

#include "windows.h"
#include <objidl.h>
#include <gdiplus.h>
#include "renderTaskPayload.h"
#include "math3d.h"

class RenderTask {

public:

	RenderTask(unsigned long seqNo, double timeStamp);
	virtual ~RenderTask();

	unsigned long getSeqNo();
	RenderTaskPayload *getPayload();

	// Accessors
	void setSliceIdx(int idx, int slices);
	void setProjectionMatrix(Matrix4f & matrix);
	Matrix4f *getProjectionMatrix();
	int getSlices();
	int getSliceIndex();
	double getTimeStamp();
	void setDimensions(int w, int h);
	int getWidth();
	int getHeight();
	void setResultBitmap(Gdiplus::Bitmap *bitmap, void *pixels);
	void getResultBitmap(Gdiplus::Bitmap **bitmap, void **pixels);

	enum TaskType {
		RT_CAMERA
	};

	Matrix4f mProjectionMatrix;

private:
	unsigned long seqNo;		 // Task sequence number
	int mSliceIdx;
	int mSlices;
	int mWidth;
	int mHeight;
	double mTimeStamp;			 // Timestamp
	// RenderTaskPayload *mPayload; // Actual task payload to be sent over the net.

	void *mFramePixels;
	Gdiplus::Bitmap *mBitmap;
};

#endif