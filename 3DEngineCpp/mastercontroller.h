// mastercontroller.h
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#ifndef MASTERCONTROLLER_H
#define MASTERCONTROLLER_H

#include <queue>
#include "frame.h"

class MasterController
{
public:
	MasterController();
	virtual ~MasterController();
private:
	std::priority_queue<Frame, std::vector<Frame>> frameQueue;
};

#endif