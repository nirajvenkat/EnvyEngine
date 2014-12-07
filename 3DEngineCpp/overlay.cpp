#include "windows.h"
#include "overlay.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

Overlay::Overlay() {
	mContent.clear();
	mColors.clear();
	mFont = NULL;

	//mWindow = window;

	mContent[0] = "Hello, world!";
	mContent[1] = "Hello, world2!";

	mFont = TTF_OpenFont(OVERLAY_FONT_PATH, 8);
	if (mFont == NULL) {
		fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
	}
}

Overlay::~Overlay() {
	if (mFont) // Dispose of font
		TTF_CloseFont(mFont);
}

void Overlay::setWindow(SDL_Window *window) {
	this->mWindow = window;
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

// Abandoned in favor of OpenFont
char *Overlay::loadFont(char *fontName) {

	char* fontData;
	size_t dataSize = 0;
	size_t bytesRead;
	size_t bytesRequested = 0;
	int block = 0;
	LPTSTR moduleName[MAX_PATH];
	errno_t err;
	FILE *fontFilePtr;

	fontData = (char*)malloc(1);

	err = fopen_s(&fontFilePtr, fontName, "rb");
	if (err == NULL) {
		fprintf(stderr, "Error opening font file.\n");
		return NULL;
	}

	while (bytesRead == bytesRequested) {
		block++;
		bytesRequested = OVERLAY_LOADBLOCKSIZE;
		fontData = (char*)realloc(fontData, block*OVERLAY_LOADBLOCKSIZE + 1);
		bytesRead = fread(fontData, 1, bytesRequested, fontFilePtr+block*OVERLAY_LOADBLOCKSIZE);
		dataSize += bytesRead;
	}
	fclose(fontFilePtr);
	fontData[dataSize] = 0;

	return fontData; // Must be freed later!
}