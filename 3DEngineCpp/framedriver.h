// framedriver.h
//
// This is a development driver for the Envy Engine frame queue to test the queue's performance and
// functionality without needing networked render nodes.

#ifndef FRAMEDRIVER_H
#define FRAMEDRIVER_H

#include "frame.h"
#include <vector>
#include <queue>

class FrameDriver {
public:

	FrameDriver(class MasterController *mc);
	virtual ~FrameDriver();
	void tick();
	void _tick();
	void loadFrames();
	bool hasFrames();
	Frame *nextFrame();
	
private:

	// Threading
	void lock();
	void unlock();

	int							   mFrameIdx;
	class MasterController		   *mc;
	std::wstring				   mFrameDir;
	std::vector<std::wstring*>     mFrameFiles;
	std::queue<class Frame*>	   mFrameQueue;

	// Threading
	struct SDL_mutex      *mTCrit;
};

#endif