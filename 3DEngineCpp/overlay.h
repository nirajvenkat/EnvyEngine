#ifndef _OVERLAY
#define _OVERLAY

#include <vector>

#define OVERLAY_FONT_PATH "..\\font\\proggytiny.ttf"
#define OVERLAY_FONT_SIZE 16
#define OVERLAY_LOADBLOCKSIZE	1024
#define OVERLAY_LINE_WIDTH	300
#define OVERLAY_LINE_HEIGHT	80
#define OVERLAY_GRAPH_WIDTH 340 // Total line width = line width + graph width
#define OVERLAY_V_MARGIN 20
#define OVERLAY_H_MARGIN 20
#define OVERLAY_GRAPH_ALPHA 64

struct _TTF_Font;
struct SDL_Color;
class SDL_Renderer;

typedef struct _OverlayLine {
	int nodeId;
	int posX;
	int posY;
	float curAvg;
	SDL_Color *color;
	SDL_Surface *surface;
	SDL_Texture *texture;
	bool textChanged;
	bool graphChanged;
	char lineText[256];
}_OverlayLine;

class Overlay {

public:

	Overlay();
	Overlay(SDL_Renderer *renderer);
	~Overlay();
	static void initializeOverlays();
	void setRenderer(SDL_Renderer *renderer);
	void clear();
	int lineCount();
	void insertLine(char *text, int lineNo);
	void render();

private:

	void _init();
	void renderLine(_OverlayLine *line, int vPos);
	char *loadFont(char *fontName);
	
	_TTF_Font *mFont;
	SDL_Renderer *mRenderer;
	std::vector<_OverlayLine*> mLines;
};

#endif