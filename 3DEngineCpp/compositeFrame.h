// compositeFrame.h
//
// A frame comprised of horizontal slices.

#ifndef COMPOSITEFRAME
#define COMPOSITEFRAME

#include "frame.h"
#include <vector>

class CompositeFrame {
public:
	CompositeFrame(int numSlices, int width, int height);
private:
	std::vector<Frame*> mFrames;
};

#endif