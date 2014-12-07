#ifndef _OVERLAY
#define _OVERLAY

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <string>
#include <vector>

class Overlay {
public:
	Overlay(struct SDL_Display *display);
	~Overlay();
	static void initializeOverlays();
	void clear();
	void insertLine(char *text, int lineNo);
	void render();
private:
	std::vector<std::string> mContent;
	std::vector<SDL_Color> mColors;
	SDL_Surface *mSurface;
};

#endif