// simulatednode.h
//
// For testing the load balancing features of MasterController and RenderNode without the network.

#ifndef SIMULATEDNODE_H
#define SIMULATEDNODE_H

#include <map>

class SimulatedNode {

public:
	
	// Statics
	static std::map<unsigned int, char*> framePool;
	static int bitmapWidth;
	static int bitmapHeight;
	static void initSimNodes(int width, int height);

	SimulatedNode(int parentId);
	void acceptTask(class RenderTask *rt);

	// Threading
	void lock();
	void unlock();

private:

	int mParentId;

	// Threading
	struct SDL_mutex      *mTCrit;
};

#endif