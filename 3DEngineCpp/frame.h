// frame.h
//
// The Envy Engine frame. For now, this consists of a bitmap which is stored in a list maintained by the master
// controller.

#ifndef FRAME_H
#define FRAME_H

class Frame {
public:
	Frame(unsigned long modelTime);
	virtual ~Frame();

	unsigned long getMaxId();
	unsigned long getModelTime();
	void setSurface(class SDL_Surface *surf);
	SDL_Surface *getSurface(void);
	unsigned long getTaskId();
private:

	static unsigned long idMax;

	class SDL_Surface *mSurface;
	unsigned long modelTime;
	unsigned long id;
	unsigned long mTaskId;

	// Going to need a place for our bitmap
};

class _FrameComparator {
public:
	bool operator() (Frame* lhs, Frame *rhs)
	{
		return lhs->getModelTime() > rhs->getModelTime();
	}
};

#endif