// frame.h
//
// The Envy Engine frame. For now, this consists of a bitmap which is stored in a list maintained by the master
// controller.

#ifndef FRAME_H
#define FRAME_H

class Frame {
public:
	Frame();
	virtual ~Frame();

	unsigned long timestamp;
	unsigned long id;

	unsigned long getMaxId();

private:
	static unsigned long idMax;

	// Gonna need a place for our bitmap
};

#endif