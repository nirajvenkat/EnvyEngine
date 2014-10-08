// renderer.cpp
//
// Envy Engine renderer. This class sits on top of any graphics-producing code in envy. For now
// it exists to handle the "software monitor" features used by the master controller.

#include "renderer.h"
#include <SDL2/SDL.h>

Renderer::Renderer() {
	mSDLRenderWindow = NULL;
	mSDLRenderer = NULL;
}
Renderer::~Renderer() {}

void Renderer::initOutputWindow(int width, int height, const char *title) 
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		// Failed to init
	}

	// Create the SDL Window
	mSDLRenderWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		0);

	// Create our renderer
	mSDLRenderer = SDL_CreateRenderer(mSDLRenderWindow, -1, SDL_RENDERER_ACCELERATED);
}