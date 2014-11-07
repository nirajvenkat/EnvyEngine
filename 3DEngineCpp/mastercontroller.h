// mastercontroller.h
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#ifndef MASTERCONTROLLER_H
#define MASTERCONTROLLER_H

#include <queue>
#include <map>
#include "frame.h"

#define MC_MAX_LATENCY 300 // In ms

class Frame;

class MasterController
{
public:
	MasterController(int frameRateMax);
	virtual ~MasterController();
	void init();
	void run();
	void addFrame(class Frame *newFrame);
	void addNode(class RenderNode *rn);
	void dropNode(unsigned int nodeId);
	void refreshNodeTimeshares();

	// Threading
	void lock();
	void unlock();

	void debugNodeStatistics();

	static int MasterController::_startMCThread(void* mc);

private:
	bool mInitialized;
	class Renderer *mRenderer;
	std::priority_queue<Frame*, std::vector<Frame*>, class _FrameComparator> mFrameQueue;	// Finished frames.
	std::queue<class RenderTask*> mWaitingTaskQueue;	// Tasks waiting to be assigned.
	std::queue<class RenderTask*> mWorkingTaskQueue;	// Tasks being worked on by active nodes.
	std::map<unsigned int, class RenderNode*> mNodes;
	int mFrameRateMax;
	unsigned int mMaxNodeId;

	// Threading
	struct SDL_Thread	  *mThread;
	struct SDL_semaphore  *mStartSem;	// Wait on this while we load up.
	struct SDL_mutex      *mTCrit;

	// Render nodes
	float *mNodeTimeshare;
	unsigned long mMaxNodeNumber;

	void _execute();
};

#endif