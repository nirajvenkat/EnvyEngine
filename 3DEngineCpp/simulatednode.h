// simulatednode.h
//
// For testing the load balancing features of MasterController and RenderNode without the network.

#ifndef SIMULATEDNODE_H
#define SIMULATEDNODE_H

#include <map>

class SimulatedNode {

public:
	static std::map<unsigned int, char*> framePool;
	static int bitmapWidth;
	static int bitmapHeight;
	static void initSimNodes(int width, int height);

	void acceptTask(class RenderTask *rt);

	// Threading
	void lock();
	void unlock();

private:
	// Threading
	struct SDL_mutex      *mTCrit;
};

#endif