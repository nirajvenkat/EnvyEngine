// simulatednode.cpp
//
// For testing the load balancing features of MasterController and RenderNode without the network.

#include "simulatednode.h"
#include "renderTask.h"
#include <SDL2/SDL_thread.h>
#include <map>

// Statics
int SimulatedNode::bitmapWidth;
int SimulatedNode::bitmapHeight;
std::map<unsigned int, char*> SimulatedNode::framePool;

SimulatedNode::SimulatedNode(int parentId) {

}

void SimulatedNode::initSimNodes(int width, int height) {
	bitmapWidth = width;
	bitmapHeight = height;
	framePool.clear();
}

void SimulatedNode::acceptTask(RenderTask *rt) {

	// Accept render task


	// Start timer

}

// Threading utility functions
void SimulatedNode::lock() {
	SDL_LockMutex(mTCrit);
}

void SimulatedNode::unlock() {
	SDL_UnlockMutex(mTCrit);
}

// Callbacks
Uint32 taskTimeout() {
	// Grab the contents of the OpenGL framebuffer. The frames don't need to be in order to start with.
	// Store the contents in the response
	// Call the response callback
}