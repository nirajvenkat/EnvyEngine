#include "sdl_backend.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>

static SDL_Window* window;
static SDL_GLContext glContext;
static bool isCloseRequested = false;

bool SDLGetIsCloseRequested()
{
	return isCloseRequested;
}

void SDLSetIsCloseRequested(bool value)
{
	isCloseRequested = value;
}

void SDLCreateWindow(const char* title, int x, int y, int width, int height, bool fullscreen)
{
	int mode = 0;

	if(fullscreen)
		mode = SDL_WINDOW_FULLSCREEN;

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | mode);
	glContext = SDL_GL_CreateContext(window);

	//SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	SDL_GL_SetSwapInterval(1);
}

void SDLSetWindowFullscreen(bool value)
{
	int mode = 0;
	if(value)
		mode = SDL_WINDOW_FULLSCREEN;
	else
		mode = 0;

	SDL_SetWindowFullscreen(window, mode);
}

void SDLSwapBuffers()
{
	SDL_GL_SwapWindow(window);
}

void SDLDestroyWindow()
{
	if(!window)
		return;

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
}

void SDLSetMousePosition(int x, int y)
{
	SDL_WarpMouseInWindow(window, x, y);
}


// Added to effect window resize tasks when node number increases or decreases
// Window and glContexts must be valid!
void SDLResizeWindow(int width, int height)
{
	//SDL_SetWindowSize(window, width, height); // Resize our window (seems to be unnecessary but check to make sure)
	glViewport(0, 0, width, height);	      // Change the GL viewport
}