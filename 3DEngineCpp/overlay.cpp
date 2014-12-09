#include "windows.h"
#include "overlay.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <assert.h>

const int Overlay::_overlayColors[OVERLAY_COLORS][3] = {
	{ 255, 255, 255 },    // White
	{ 255, 155, 255 },    // Purple
	{ 155, 255, 155 },    // Green
	{ 200, 200, 0 },      // Yellow
	{ 0x7F, 0xFF, 0x44 }  // Lt Chartreuse
};

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

void Overlay::clear() {
	for (std::vector<_OverlayLine*>::iterator it = mLines.begin(); it != mLines.end(); ++it) {
		freeLine(*it);
	}
	mLines.clear();
}

int Overlay::lineCount() {
	return mLines.size();
}

void Overlay::insertLine(int nodeId) {

	_OverlayLine *newLine;
	SDL_Color *color;
	const int *thisColor = _overlayColors[(nodeId-1) % OVERLAY_COLORS];

	// Allocate new line
	color = (SDL_Color*)malloc(sizeof(SDL_Color));
	newLine = (_OverlayLine*)malloc(sizeof(_OverlayLine));
	if (color && newLine) {
		newLine->nodeId = nodeId;
		newLine->curGraphIdx = 0;
		newLine->curAvg = 0.0f;
		newLine->color = color;
		*newLine->color = { thisColor[0], thisColor[1], thisColor[2] }; // TEMP
		newLine->textChanged = false;
		newLine->graphChanged = false;
		newLine->text[0] = 0;

		// Create textures
		newLine->texture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888,
										SDL_TEXTUREACCESS_TARGET,
										OVERLAY_GRAPH_WIDTH + OVERLAY_TEXT_WIDTH + OVERLAY_H_MARGIN,
										OVERLAY_LINE_HEIGHT);
		for (int i = 0; i < 2; i++)
				newLine->graphs[i] = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_TARGET,
				OVERLAY_GRAPH_WIDTH,
				OVERLAY_LINE_HEIGHT);

		if (!newLine->texture || !newLine->graphs[0] || !newLine->graphs[1])
			fprintf(stderr, "Error creating texture: %s", SDL_GetError());
		else
			mLines.push_back(newLine);
	}
	else {
		fprintf(stderr, "The Overlay could not allocate storage for a new line!\n");
	}
}

void Overlay::removeLine(int nodeId) {
	_OverlayLine *curLine = getLine(nodeId);
	if (curLine) {
		for (std::vector<_OverlayLine*>::iterator it = mLines.begin(); it != mLines.end(); ++it) {
			if (*it == curLine) {
				mLines.erase(it);
				freeLine(curLine);
				return;
			}
		}
	}
}

void Overlay::updateAvg(int nodeId, float avg) {
	_OverlayLine *curLine = getLine(nodeId);
	if (curLine) {
		curLine->curAvg = avg;
		curLine->graphChanged = true;
	}
}

void Overlay::updateText(int nodeId, const char *text) {
	_OverlayLine *curLine = getLine(nodeId);
	if (curLine) {
		strcpy_s(curLine->text, OVERLAY_MAX_TEXT, text);
		curLine->textChanged = true;
	}
}

void Overlay::render() {

	int vPos = 0;
	for (std::vector<_OverlayLine*>::iterator it = mLines.begin(); it != mLines.end(); ++it) {
		_OverlayLine *curLine = *it;
		renderLine(curLine, vPos++);
	}
}

void Overlay::_init() {
	mLines.clear();
	mFont = NULL;

	mFont = TTF_OpenFont(OVERLAY_FONT_PATH, OVERLAY_FONT_SIZE);
	if (mFont == NULL) {
		fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
	}
}

_OverlayLine *Overlay::getLine(int nodeId) {
	for (std::vector<_OverlayLine*>::iterator it = mLines.begin(); it != mLines.end(); ++it)
		if ((*it)->nodeId == nodeId)
			return *it;
	return NULL;
}

void Overlay::renderLine(_OverlayLine *line, int vPos) {
	
	int pg;
	int cg;
	SDL_Rect srcRect;
	SDL_Rect dstRect;

	// Graph swap textures
	pg = line->curGraphIdx;
	line->curGraphIdx = (line->curGraphIdx + 1) % 2;
	cg = line->curGraphIdx;

	if (line->textChanged || line->graphChanged) {
		// Fill line texture with black
		SDL_SetRenderTarget(mRenderer, line->texture); // Important for the second part!
		SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
	}

	if (line->textChanged) { // Render text portion

		SDL_Rect textRect;	// Construct text area rectangle + margin	
		SDL_Texture *textTexture;
		SDL_Surface *textSurface;
		
		dstRect.y = OVERLAY_LINE_HEIGHT - OVERLAY_TEXT_HEIGHT;
		dstRect.x = 0;

		textRect.x = 0;
		textRect.y = 0;
		textRect.w = OVERLAY_TEXT_WIDTH + OVERLAY_H_MARGIN;
		textRect.h = OVERLAY_LINE_HEIGHT;

		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0);
		SDL_RenderFillRect(mRenderer, &textRect); // Black out area

		textSurface = TTF_RenderText_Solid(mFont, line->text, *(line->color));
		if (!textSurface) {
			fprintf(stderr, "Error rendering text: %s\n", TTF_GetError());
		}

		// Create a texture from the font surface and copy it into the main line texture
		textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		memcpy(&srcRect, &textSurface->clip_rect, sizeof(SDL_Rect));
		dstRect.w = srcRect.w;
		dstRect.h = srcRect.h;
		SDL_RenderCopy(mRenderer, textTexture, &srcRect, &dstRect);

		// Dispose text texture and surfaces
		SDL_DestroyTexture(textTexture);
		SDL_FreeSurface(textSurface);

		// Reset changed flag
		line->textChanged = false;
	}
	if (line->graphChanged) { // Render graph portion
		
		SDL_Rect barRect;

		// Set new bar dimensions
		barRect.x = 0;
		barRect.y = OVERLAY_LINE_HEIGHT*(1.0f - line->curAvg); // 0.0f - 1.0f
		barRect.w = OVERLAY_GRAPH_BAR_WIDTH - 1;
		barRect.h = OVERLAY_LINE_HEIGHT - barRect.y;

		// Blackfill dimensions
		srcRect.x = OVERLAY_GRAPH_BAR_WIDTH - 1;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = OVERLAY_LINE_HEIGHT;
		dstRect.x = barRect.x;
		dstRect.y = 0;
		dstRect.w = OVERLAY_GRAPH_BAR_WIDTH;
		dstRect.h = OVERLAY_LINE_HEIGHT - barRect.h;

		SDL_SetRenderTarget(mRenderer, line->graphs[cg]);

		// Fill black
		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0);
		SDL_RenderFillRect(mRenderer, &srcRect);
		SDL_RenderFillRect(mRenderer, &dstRect);

		// Fill new graph bar
		SDL_SetRenderDrawColor(mRenderer, line->color->r, 
			                              line->color->g, 
										  line->color->b, 
										  line->color->a);
		SDL_RenderFillRect(mRenderer, &barRect);

		// Slide old bars to the right and clip the last one.
		srcRect.x = 0;
		dstRect.x = OVERLAY_GRAPH_BAR_WIDTH;
		dstRect.y = srcRect.y = 0;
		srcRect.w = dstRect.w = OVERLAY_GRAPH_WIDTH - OVERLAY_GRAPH_BAR_WIDTH;
		dstRect.h = srcRect.h = OVERLAY_LINE_HEIGHT;
		
		// Copy the clipped portion to the new graph
		SDL_RenderCopy(mRenderer, line->graphs[pg], &srcRect, &dstRect);

		// Copy the new graph to the whole line texture
		SDL_SetRenderTarget(mRenderer, line->texture);
		dstRect.w = srcRect.w = OVERLAY_GRAPH_WIDTH;
		dstRect.x = srcRect.x + OVERLAY_TEXT_WIDTH + OVERLAY_H_MARGIN; // Right of text portion
		SDL_RenderCopy(mRenderer, line->graphs[cg], &srcRect, &dstRect);

		line->graphChanged = false;
	}
	
	// Render to screen
	SDL_SetRenderTarget(mRenderer, NULL); // Restore render target
	
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = OVERLAY_TEXT_WIDTH + OVERLAY_GRAPH_WIDTH + OVERLAY_H_MARGIN;
	srcRect.h = OVERLAY_LINE_HEIGHT;

	dstRect.x = OVERLAY_POS_X;
	dstRect.y = OVERLAY_POS_Y + vPos*OVERLAY_LINE_HEIGHT
							  + vPos*OVERLAY_V_MARGIN;
	dstRect.h = srcRect.h;
	dstRect.w = srcRect.w;

	SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
	SDL_RenderCopy(mRenderer, line->texture, &srcRect, &dstRect); // Final blit
}

void Overlay::freeLine(_OverlayLine *line) {
	for (int i = 0; i < 2; i++)
		SDL_DestroyTexture(line->graphs[i]);
	SDL_DestroyTexture(line->texture);
	free(line->color);
	free(line);
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
