#ifndef _OVERLAY
#define _OVERLAY

#include <vector>

#define OVERLAY_POS_X 10
#define OVERLAY_POS_Y 10
#define OVERLAY_FONT_PATH "..\\font\\proggytiny.ttf"
#define OVERLAY_FONT_SIZE 16
#define OVERLAY_LOADBLOCKSIZE	1024
#define OVERLAY_TEXT_HEIGHT 9
#define OVERLAY_TEXT_WIDTH	150
#define OVERLAY_LINE_HEIGHT	15
#define OVERLAY_GRAPH_WIDTH 340 // Total line width = line width + graph width
#define OVERLAY_V_MARGIN 1
#define OVERLAY_H_MARGIN 20
#define OVERLAY_GRAPH_ALPHA 64
#define OVERLAY_GRAPH_BAR_WIDTH 2
#define OVERLAY_MAX_TEXT 256
#define OVERLAY_COLORS 5

struct _TTF_Font;
struct SDL_Color;
struct SDL_Surface;
struct SDL_Texture;
class SDL_Renderer;

typedef struct _OverlayLine {
	int nodeId;
	int curGraphIdx;
	float curAvg;
	SDL_Color *color;
	SDL_Texture *texture;
	SDL_Texture *graphs[2];
	bool textChanged;
	bool graphChanged;
	char text[OVERLAY_MAX_TEXT];
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
	void insertLine(int nodeId);
	void removeLine(int nodeId);
	void updateAvg(int nodeId, float avg);
	void updateText(int nodeId, const char *text);
	void render();
private:

	void _init();
	_OverlayLine *getLine(int nodeId);
	void renderLine(_OverlayLine *line, int vPos);
	void freeLine(_OverlayLine *line);
	char *loadFont(char *fontName);
	
	_TTF_Font *mFont;
	SDL_Renderer *mRenderer;
	std::vector<_OverlayLine*> mLines;

	static const int _overlayColors[OVERLAY_COLORS][3];
};

#endif