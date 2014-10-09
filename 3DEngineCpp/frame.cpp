// frame.cpp
//
// The Envy Engine frame. For now, this consists of a bitmap which is stored in a list maintained by the master
// controller.

#include "frame.h"
#include <SDL2/SDL.h>

unsigned long Frame::idMax = 1;

Frame::Frame(unsigned long modeltime)
{
	mSurface = NULL;
	this->modelTime = modeltime;
	this->id = idMax++;
}

Frame::~Frame() {
	if (mSurface)
		SDL_FreeSurface(mSurface); // Free the surface
}

void Frame::setSurface(SDL_Surface *surf) {
	mSurface = surf;
}

SDL_Surface *Frame::getSurface() {
	return mSurface;
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