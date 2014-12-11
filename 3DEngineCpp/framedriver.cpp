// framedriver.cpp
//
// This is a development driver for the Envy Engine frame queue to test the queue's performance and
// functionality without needing networked render nodes.

#define FRAME_DIR L"..\\3DEngineCpp\\res\\testframes"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include "frame.h"
#include "framedriver.h"
#include "mastercontroller.h"
#include "windows.h" // For windows file I/O mainly

FrameDriver::FrameDriver(MasterController *mc) {
	this->mc = mc; // MasterController to feed frames to
	mFrameIdx = 0;

	mTCrit = SDL_CreateMutex();
	if (mTCrit == NULL) {
		fprintf(stderr, "Could not create SDL_Mutex: %s\n", SDL_GetError());
		return;
	}
}

FrameDriver::~FrameDriver() {
	SDL_DestroyMutex(mTCrit);
}

// For sorting wstrings
bool _wstringptr_sort(std::wstring *i, std::wstring *j) {
	return (*i < *j); // This is strcmp for dummies
}

void FrameDriver::loadFrames() {
	
	HANDLE hFind;
	WIN32_FIND_DATA findData;
	SDL_Surface *curSurf;
	TCHAR imgPath[MAX_PATH];
	char aImgPath[MAX_PATH];
	TCHAR pathBuf[MAX_PATH];
	std::wstring str;
	size_t pos;

	// Path of executable
	GetModuleFileNameW(NULL, pathBuf, MAX_PATH);
	
	// Snip executable name.
	str = pathBuf;
	pos = str.find_last_of(L"\\", std::wstring::npos);
	if (pos < MAX_PATH - 1) {
		pathBuf[pos + 1] = 0;
	}
	lstrcatW(pathBuf, FRAME_DIR);
	mFrameDir = pathBuf; // Set frame directory

	lstrcpy(imgPath, pathBuf);
	lstrcat(imgPath, L"\\*.png"); // Search for PNG graphics

	// Load all test frames out of frame directory
	mFrameFiles.clear();

	// Anything that has a .png extension. Sort in alpha order.
	hFind = FindFirstFileW(imgPath, &findData);
	mFrameFiles.push_back(new std::wstring(findData.cFileName));

	while (FindNextFileW(hFind, &findData))
		mFrameFiles.push_back(new std::wstring(findData.cFileName));

	std::sort(mFrameFiles.begin(), mFrameFiles.end(), _wstringptr_sort);

	// Load textures
	fprintf(stderr, "FrameDriver found %d surfaces for use.\n", mFrameFiles.size());

	/*
	for (std::vector<std::wstring*>::iterator it = mFrameFiles.begin(); it != mFrameFiles.end(); it++) {

		lstrcpy(imgPath, pathBuf);
		lstrcat(imgPath, L"\\");
		lstrcat(imgPath, (*it)->c_str());

		wcstombs(aImgPath, imgPath, MAX_PATH);

		curSurf = IMG_Load(aImgPath);

		if (!curSurf) {
			fprintf(stderr, "Error loading surface: %s\n", SDL_GetError());
		}
		else {
			mSurfaceHolder.push_back(curSurf);
		}
	}*/
}

void FrameDriver::_tick() {

	size_t bufSize = 4 * 1366 * 720;

	lock();

	if (mFrameQueue.size() > 256) {
		unlock();
		return;
	}

	GLubyte *frameBufData = (GLubyte*)malloc(bufSize);
	if (frameBufData) // We've run out of memory in tests before.
		memset(frameBufData, 0, bufSize);

	// Create a surface
	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(frameBufData, 1366, 720, 32, 1366*4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	if (!surface) {
		fprintf(stderr, "Error creating surface (possibly out of memory): %s\n", SDL_GetError());
	}

	if (frameBufData && surface) {
		glReadPixels(0, 0, 1366, 720, GL_RGBA, GL_UNSIGNED_BYTE, frameBufData);

		SDL_Rect rect;
		rect.x = rect.y = 0;
		rect.w = 1366;
		rect.h = 720;

		Frame *nFrame = new Frame(&rect, mFrameIdx++);
		nFrame->setSurface(surface);
		mFrameQueue.push(nFrame);

	}

	unlock();
}

bool FrameDriver::hasFrames() 
{
	lock();
	int queueSize = mFrameQueue.size();
	unlock();
	return (queueSize > 0);
}

Frame *FrameDriver::nextFrame()
{
	lock();
	Frame *outFrame = mFrameQueue.front();
	mFrameQueue.pop();
	unlock();
	return outFrame;
}

void FrameDriver::tick() {
	/* From Sprint 1. Candidate for deletion.
	static unsigned int idx = 0;

	Frame *curFrame = new Frame(idx++); 
	SDL_Surface *curSurf;
	char imgPath[MAX_PATH];
	char imgName[MAX_PATH];

	// Check time since last tick
	// Over target rate?

	// Add frame to master controller's queue.
	wcstombs(imgPath, mFrameDir.data(), MAX_PATH);
	wcstombs(imgName, mFrameFiles[mFrameIdx++]->data(), MAX_PATH);

	strcat(imgPath, "\\");
	strcat(imgPath, imgName);

	curSurf = IMG_Load(imgPath);
	if (curSurf) {
		curFrame->setSurface(curSurf);
		mc->addFrame(curFrame);
	}

	if (mFrameIdx >= mFrameFiles.size())
		mFrameIdx = 0;
		*/
}

// Threading
void FrameDriver::lock()
{
	SDL_LockMutex(mTCrit);
}
void FrameDriver::unlock()
{
	SDL_UnlockMutex(mTCrit);
}