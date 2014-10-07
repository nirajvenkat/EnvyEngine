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

	// Video monitor for master controller tasks
	void initOutputWindow(int width, int height, const char *title);
	void displayFrame(Frame *f);

private:
	class SDL_Window *sdlRenderWindow;
	class SDL_Renderer *sdlRenderer;
};

#endif