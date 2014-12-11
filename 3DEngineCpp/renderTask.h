// renderTask.h
//
// RenderTask tells the renderer what to render. The task payload is sent over the network to individual hardware nodes, which render frames
// according to the parameters in the task payload. Render tasks are also used to direct nodes to do things like load world data and adjust overall
// rendering parameters. The taskType attribute determines what kind of task is being sent.

#ifndef RENDERTASK_H
#define RENDERTASK_H

#include "renderTaskPayload.h"
#include "math3d.h"

class RenderTask {

public:

	RenderTask(unsigned long seqNo, double timeStamp);
	virtual ~RenderTask();

	unsigned long getSeqNo();
	RenderTaskPayload *getPayload();
	void setPayload(int taskType, size_t payloadSize, void *payloadData);
	void clearPayload();

	// Accessors
	void setSliceIdx(int idx, int slices);
	void setProjectionMatrix(Matrix4f & matrix);
	Matrix4f *getProjectionMatrix();
	int getSlices();
	int getSliceIndex();
	double getTimeStamp();

	enum TaskType {
		RT_CAMERA
	};

	Matrix4f mProjectionMatrix;

	// Data types for render tasks
	//typedef double SimpleMat4[4][4]; // Simple 4x4 matrix for task type 0
	//typedef struct CameraTaskData {
	//	SimpleMat4	view;
	//	SimpleMat4	projection;
	//} CameraTaskData;

private:
	unsigned long seqNo;		 // Task sequence number
	int mSliceIdx;
	int mSlices;
	double mTimeStamp;			 // Timestamp
	RenderTaskPayload *mPayload; // Actual task payload to be sent over the net.
};

#endif