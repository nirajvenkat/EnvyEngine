// frame.cpp
//
// The Envy Engine frame. For now, this consists of a bitmap which is stored in a list maintained by the master
// controller.

#include "frame.h"
#include <string.h>
#include <SDL2/SDL.h>

unsigned long Frame::idMax = 1;

Frame::Frame(SDL_Rect *rect, float modeltime)
{
	mSurface = NULL;
	this->modelTime = modeltime;
	this->id = idMax++;

	// Copy frame rect
	this->rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	memcpy(&this->rect, rect, sizeof(SDL_Rect));
}

Frame::~Frame() {
	if (mSurface && mSurface->pixels)
		free(mSurface->pixels); // In SDL, if we're responsible for creating the surface from our own bitmap, so we have to dispose of it.
	if (mSurface)
		SDL_FreeSurface(mSurface); // Free the surface
	free(this->rect);
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

unsigned long Frame::getModelTime()
{
	return modelTime;
}

unsigned long Frame::getTaskId() {
	return mTaskId;
}