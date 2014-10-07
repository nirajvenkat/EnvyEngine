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
	inline unsigned long getModelTime();

private:

	static unsigned long idMax;

	class SDL_Surface *surface;
	unsigned long modelTime;
	unsigned long id;

	// Going to need a place for our bitmap
};

#endif