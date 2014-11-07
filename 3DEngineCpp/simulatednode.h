// simulatednode.h
//
// For testing the load balancing features of MasterController and RenderNode without the network.

#ifndef SIMULATEDNODE_H
#define SIMULATEDNODE_H

#include <map>
#include <SDL2/SDL_timer.h>

class SimulatedNode {

public:

	SimulatedNode(class RenderNode *parent, int minLatency, int maxLatency, int dropTime);

	// Statics
	static std::map<unsigned int, char*> framePool;
	static int bitmapWidth;
	static int bitmapHeight;
	static void initSimNodes(int width, int height);

	void acceptTask(class RenderTask *rt);
	void finishTask();
	int getId();

	// Threading
	void lock();
	void unlock();

private:

	// Parent
	class RenderNode *mParent;

	int mParentId;
	int mMinLatency;
	int mMaxLatency;
	int mDropTime;
	unsigned int mTasksDone;

	// Threading
	struct SDL_mutex      *mTCrit;
	SDL_TimerID mTid;
};

#endif