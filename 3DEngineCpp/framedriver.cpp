// framedriver.cpp
//
// This is a development driver for the Envy Engine frame queue to test the queue's performance and
// functionality without needing networked render nodes.

#define FRAME_DIR "res\\testframes"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "framedriver.h"
#include "windows.h" // For windows file I/O mainly

FrameDriver::FrameDriver(MasterController *mc) {
	this->mc = mc; // MasterController to feed frames to



}

void FrameDriver::loadFrames() {
	
	wchar_t pathBuf[MAX_PATH];
	std::wstring str;
	size_t pos;

	// Path of executable
	GetModuleFileNameW(NULL, pathBuf, MAX_PATH);
	
	// Snip executable name.
	pos = str.find_last_of(L"\\", std::wstring::npos);
	if (pos < MAX_PATH - 1) {
		pathBuf[pos + 1] = 0;
	}

	// Load all test frames out of frame directory
	// Anything that has a .png extension. Sort in alpha order.
}

void FrameDriver::_tick() {

	// Note: Currently unused
	size_t bufSize = 4 * 1366 * 720;
	GLubyte *frameBufData = (GLubyte*)malloc(bufSize);
	memset(frameBufData, 0, bufSize);


	// Create a surface
	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(frameBufData, 1366, 720, 32, 1366*4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	if (!surface) {
	fprintf(stderr, "Error creating surface: %s\n", SDL_GetError());
	}

	if (frameBufData && surface) {
	glReadPixels(0, 0, 1366, 720, GL_RGBA, GL_UNSIGNED_BYTE, frameBufData);
	}

	if (SDL_SaveBMP(surface, "D:\\test.bmp")) {
	fprintf(stderr, "Error saving BMP: %s\n", SDL_GetError());
	}
	free(frameBufData);
}

void FrameDriver::tick() {
	// Check time since last tick
	// Over target rate?
		// Add frame to master controller's queue.
}

FrameDriver::~FrameDriver(){}
