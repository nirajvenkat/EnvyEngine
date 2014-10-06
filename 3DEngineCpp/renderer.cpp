// renderer.cpp
//
// Envy Engine renderer. This class sits on top of any graphics-producing code in envy. For now
// it exists to handle the "software monitor" features used by the master controller.

#include "renderer.h"

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::initOutputWindow(int width, int height, const char *title) 
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		// Failed to init
	}

	// Create the SDL Window
	this->sdlRenderWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		0);

	// Create our renderer
	this->sdlRenderer = SDL_CreateRenderer(this->sdlRenderWindow, -1, SDL_RENDERER_ACCELERATED);
}