#include "windows.h"
#include "overlay.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <assert.h>

Overlay::Overlay() {
	_init();
}

Overlay::Overlay(SDL_Renderer *renderer) {
	_init();
	setRenderer(renderer);
}

Overlay::~Overlay() {
	if (mFont) // Dispose of font
		TTF_CloseFont(mFont);
}

void Overlay::_init() {
	mLines.clear();
	mFont = NULL;

	mFont = TTF_OpenFont(OVERLAY_FONT_PATH, OVERLAY_FONT_SIZE);
	if (mFont == NULL) {
		fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
	}

	/* Test text
	SDL_Color color = { 255, 255, 255 };
	SDL_Surface *text = TTF_RenderText_Solid(mFont, "EnvyEngine 1.0 - It's happening!", color);

	mTextSurf.
	mTextSurf.push_back(text);
	mColors.push_back(color);

	assert(text != NULL);
	*/
}

void Overlay::setRenderer(SDL_Renderer *renderer) {
	this->mRenderer = renderer;
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

void Overlay::render() {
	//SDL_Surface *text = mText[0];
	//SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, text);
	//SDL_RenderCopy(mRenderer, texture, &text->clip_rect, &text->clip_rect);
	//SDL_DestroyTexture(texture);
}


void renderLine(_OverlayLine *line, int vPos) {
	
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
