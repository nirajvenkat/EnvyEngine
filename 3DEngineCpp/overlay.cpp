#include "overlay.h"


Overlay::Overlay(SDL_Display *display) {
	mContent.clear();
	mColors.clear();
}

Overlay::~Overlay() {

}

void Overlay::initializeOverlays() {

	int err;

	// Initialize SDL_ttf and everything else required to render fonts to texture.
	if (!TTF_WasInit()) {
		err = TTF_Init();
		if (err != 0) {
			fprintf(stderr, "Error Initializing TTF: %s\n", TTF_GetError());
		}
	}
}

