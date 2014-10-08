// framedriver.h
//
// This is a development driver for the Envy Engine frame queue to test the queue's performance and
// functionality without needing networked render nodes.

#ifndef FRAMEDRIVER_H
#define FRAMEDRIVER_H

#include "frame.h"
#include <queue>

class FrameDriver {
public:
	FrameDriver(class MasterController *mc);
	virtual ~FrameDriver();
	void tick();
	void _tick();

private:
	class MasterController		*mc;
	std::queue<class Frame*>	frameHolder;
};

#endif