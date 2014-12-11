// mastercontroller.h
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#ifndef MASTERCONTROLLER_H
#define MASTERCONTROLLER_H

#include <queue>
#include <set>
#include <map>
#include "frame.h"

#define MC_MAX_LATENCY 300 // In ms
//#define TEST_MC // Uncomment for MC test mode

class Frame;

class MasterController
{
public:

	MasterController(int frameRateMax, class Game *game);
	virtual ~MasterController();
	void init(int width, int height);
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
	std::set<class RenderTask*> mWaitingTasks;	// Tasks waiting to be assigned.
	std::set<class RenderTask*> mWorkingTasks;	// Tasks being worked on by active nodes.
	std::map<unsigned int, class RenderNode*> mNodes;
	std::map<unsigned int, class RenderTask*> mNodeTaskMap; // Working node->task map
	int mFrameRateMax;
	double mMinFramePeriod;
	double mFramePeriod;
	double mWaitPeriod;
	double mFrameTime;
	unsigned int mMaxNodeId;
	unsigned long mLastTaskId;
	class Game *mGame;

	double curTime;

	// Threading
	struct SDL_Thread	  *mThread;
	struct SDL_semaphore  *mStartSem;	// Wait on this while we load up.
	struct SDL_mutex      *mTCrit;

	// Render nodes
	float *mNodeTimeshare;
	unsigned long mMaxNodeNumber;

	void _execute();
	RenderTask *nextWaitingTask();
	void assignTask(class RenderNode *node, class RenderTask *task);
	void unassignTask(class RenderNode *node, class RenderTask *task);
	void resetWork();
	void clearWaitingTasks();
	void clearWorkingTasks();
	void clearFrames();
};

#endif