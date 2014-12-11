// renderer.h
//
// Envy Engine renderer. This class sits on top of any graphics-producing code in envy. For now
// it exists to handle the "software monitor" features used by the master controller.

#ifndef RENDERER_H
#define RENDERER_H

#include "frame.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <GdiPlusImageCodec.h>

#define RENDERER_OUTPUT_WINDOW_DEFAULT_TITLE "Envy Engine Output"

class Renderer {
public:
	Renderer(class Camera *gCamera);
	virtual ~Renderer();
	void renderFrame(class Frame *frame);

	// Video monitor for master controller tasks
	void initOutputWindow(int width, int height, const char *title);

	// Framebuffer extraction
	Gdiplus::Bitmap *getFrameBuffer(void **pixels);

	// Accessors
	int getHeight();
	int getWidth();

	// Overlay passthrough
	void addNodeToOverlay(int nodeId);
	void removeNodeFromOverlay(int nodeId);
	void updateNodeOnOverlay(int nodeId, const char *text, float avg);
	void updateViewportForTask(class RenderTask *task);

private:

	int mRenderWidth;
	int mRenderHeight;

	class Camera *mCamera;
	class SDL_Display *mDisplay;
	class Overlay *mOverlay;
	class SDL_Window *mSDLRenderWindow;
	class SDL_Renderer *mSDLRenderer;
};

#endif