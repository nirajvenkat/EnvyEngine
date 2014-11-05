// renderTaskFactory.h
//
// The render task factory creates render tasks based on the type of task required. The new render task is then returned.

#include "renderTask.h"

#ifndef RENDERTASKFACTORY_H
#define RENDERTASKFACTORY_H

class RenderTaskFactory {
public:
	static class RenderTask *createCameraTask(unsigned int seqNo, const RenderTask::SimpleMat4 viewMatrix, const RenderTask::SimpleMat4 projectionMatrix);
};

#endif