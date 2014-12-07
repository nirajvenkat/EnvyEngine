#ifndef _OVERLAY
#define _OVERLAY

#include <string>
#include <vector>

#define OVERLAY_FONT_PATH "..\\data-latin.ttf"
#define OVERLAY_FONT_SIZE 10
#define OVERLAY_LOADBLOCKSIZE	1024

class Overlay {

public:

	Overlay();
	~Overlay();
	static void initializeOverlays();
	void setWindow(struct SDL_Window *window);
	void clear();
	void insertLine(char *text, int lineNo);
	void render();

private:

	char *loadFont(char *fontName);
	
	struct _TTF_Font *mFont;
	struct SDL_Window *mWindow;
	std::vector<std::string> mContent;
	std::vector<struct SDL_Color> mColors;
	std::vector<struct SDL_Surface*> mSurfaces;
};

#endif