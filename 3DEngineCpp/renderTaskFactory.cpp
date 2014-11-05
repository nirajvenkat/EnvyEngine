// renderTaskFactory.cpp
//
// The render task factory creates render tasks based on the type of task required. The new render task is then returned.

#include "renderTask.h"
#include "renderTaskFactory.h"
#include <string.h>

class RenderTask *RenderTaskFactory::createCameraTask(unsigned int seqNo, const RenderTask::SimpleMat4 viewMatrix, const RenderTask::SimpleMat4 projectionMatrix) {
	RenderTask *rt;
	RenderTask::CameraTaskData ctd;

	rt = new RenderTask(seqNo);
	
	// Load task data
	memcpy(&ctd.projection, projectionMatrix, sizeof(RenderTask::SimpleMat4));
	memcpy(&ctd.view, viewMatrix, sizeof(RenderTask::SimpleMat4));

	rt->setPayload(RenderTask::RT_CAMERA, sizeof(ctd), &ctd);

	return rt;
}

