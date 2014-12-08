// renderer.h
//
// Envy Engine renderer. This class sits on top of any graphics-producing code in envy. For now
// it exists to handle the "software monitor" features used by the master controller.

#ifndef RENDERER_H
#define RENDERER_H

#include "frame.h"

#define RENDERER_OUTPUT_WINDOW_DEFAULT_TITLE "Envy Engine Output"

class Renderer {
public:
	Renderer();
	virtual ~Renderer();
	void renderFrame(class Frame *frame);

	// Video monitor for master controller tasks
	void initOutputWindow(int width, int height, const char *title);
	void displayFrame(Frame *f);

private:
	class SDL_Display *mDisplay;
	class Overlay *mOverlay;
	class SDL_Window *mSDLRenderWindow;
	class SDL_Renderer *mSDLRenderer;
};

#endif