// renderTaskFactory.h
//
// The render task factory creates render tasks based on the type of task required. The new render task is then returned.

#include <GL/glew.h>
#include <glm/glm.hpp>

#ifndef RENDERTASKFACTORY_H
#define RENDERTASKFACTORY_H

class RenderTaskFactory {
	static class RenderTask *createViewTask(glm::mat4 *viewMatrix);
};

#endif