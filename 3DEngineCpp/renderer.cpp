// renderer.cpp
//
// Envy Engine renderer. This class sits on top of any graphics-producing code in envy. For now
// it exists to handle the "software monitor" features used by the master controller.

#include "renderer.h"
#include "frame.h"
#include "overlay.h"
#include <SDL2/SDL.h>

Renderer::Renderer() {
	mSDLRenderWindow = NULL;
	mSDLRenderer = NULL;
}
Renderer::~Renderer() {
	delete mOverlay;
}

void Renderer::initOutputWindow(int width, int height, const char *title) 
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) { // Timer for SimNodes
		// Failed to init
	}

	// Create the SDL Window
	mSDLRenderWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		0);

	// Create our renderer
	mSDLRenderer = SDL_CreateRenderer(mSDLRenderWindow, -1, SDL_RENDERER_ACCELERATED);

	// Set up overlay	
	Overlay::initializeOverlays();
	mOverlay = new Overlay(mSDLRenderer);
}

void Renderer::renderFrame(Frame *frame) {

	SDL_Rect destRect;

	SDL_Surface *surf = frame->getSurface();
	SDL_Texture *tex = SDL_CreateTextureFromSurface(mSDLRenderer, surf);

	// Use surface's width and height
	destRect.h = surf->h;
	destRect.w = surf->w;
	destRect.x = 100; //TODO: Change
	destRect.y = 100; //TODO: Change

	SDL_RenderCopy(mSDLRenderer, tex, NULL, NULL);
	mOverlay->render();
	SDL_RenderPresent(mSDLRenderer);
	SDL_DestroyTexture(tex);

}

// Overlay passthrough functions
void Renderer::addNodeToOverlay(int nodeId) {
	mOverlay->insertLine(nodeId);
}

void Renderer::removeNodeFromOverlay(int nodeId) {
	mOverlay->removeLine(nodeId);
}

void Renderer::updateNodeOnOverlay(int nodeId, const char *text, float avg) {
	mOverlay->updateText(nodeId, text);
	mOverlay->updateAvg(nodeId, avg);
}