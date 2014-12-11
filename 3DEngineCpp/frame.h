// frame.h
//
// The Envy Engine frame. For now, this consists of a bitmap which is stored in a list maintained by the master
// controller.

#ifndef FRAME_H
#define FRAME_H

class Frame {
public:
	Frame(struct SDL_Rect *rect, float modelTime);
	virtual ~Frame();

	unsigned long getMaxId();
	unsigned long getModelTime();
	void setSurface(struct SDL_Surface *surf);
	struct SDL_Surface *getSurface(void);
	unsigned long getTaskId();
	struct SDL_Rect *getRect();
private:

	struct SDL_Rect *rect;
	static unsigned long idMax;
	struct SDL_Surface *mSurface;
	unsigned long modelTime;
	unsigned long id;
	unsigned long mTaskId;
};

class _FrameComparator {
public:
	bool operator() (Frame* lhs, Frame *rhs)
	{
		return lhs->getModelTime() > rhs->getModelTime();
	}
};

#endif