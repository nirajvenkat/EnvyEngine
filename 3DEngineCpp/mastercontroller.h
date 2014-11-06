// mastercontroller.h
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#ifndef MASTERCONTROLLER_H
#define MASTERCONTROLLER_H

#include <queue>
#include <map>
#include "frame.h"

class Frame;

class MasterController
{
public:
	MasterController(int frameRateMax);
	virtual ~MasterController();
	void init();
	void run();
	void addFrame(Frame *newFrame);
	void addNode();
	void refreshNodeTimeshares();

	// Threading
	void lock();
	void unlock();

	static int MasterController::_startMCThread(void* mc);

private:
	bool mInitialized;
	class Renderer *mRenderer;
	std::priority_queue<Frame*, std::vector<Frame*>, class _FrameComparator> mFrameQueue;
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