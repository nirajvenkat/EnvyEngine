// frame.cpp
//
// The Envy Engine frame. For now, this consists of a bitmap which is stored in a list maintained by the master
// controller.

#include "frame.h"

unsigned long Frame::idMax = 1;

Frame::Frame()
{
	this->modelTime = 0; // Temp
	this->id = idMax++;
}

Frame::~Frame() {

}

unsigned long Frame::getMaxId()
{
	return idMax;
}

inline unsigned long Frame::getModelTime()
{
	return modelTime;
}

bool operator<(Frame& lhs, Frame& rhs)
{
	return lhs.getModelTime() < rhs.getModelTime();
}

bool operator>(Frame& lhs, Frame& rhs)
{
	return lhs.getModelTime() > rhs.getModelTime();
}