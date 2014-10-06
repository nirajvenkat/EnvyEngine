// mastercontroller.h
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#ifndef MASTERCONTROLLER_H
#define MASTERCONTROLLER_H

#include <queue>
#include "frame.h"
#include "windows.h"

class MasterController
{
public:
	MasterController();
	virtual ~MasterController();
	void init();
	void run();

	// Threading
	void lock();
	void unlock();
	void addFrame(Frame *newFrame);

private:
	std::priority_queue<Frame*, std::vector<Frame*>> frameQueue;

	// Threading
	CRITICAL_SECTION tcrit;
};

#endif