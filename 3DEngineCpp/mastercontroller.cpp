// mastercontroller.cpp
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#include "mastercontroller.h"
#include "windows.h"
#include <queue>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

using namespace std;

// Entry point for a new MC thread.
int MasterController::_startMCThread(void* mc) {
	((MasterController*)mc)->_execute();
	return 0;
}

MasterController::MasterController(int frameRateMax) {
	mFrameRateMax = frameRateMax;
	mStartSem = NULL;
}

// Destroy the master controller in a thread-safe manner.
MasterController::~MasterController() {

	SDL_LockMutex(mTCrit);

	if (mInitialized) {
		SDL_DestroySemaphore(mStartSem);
	}
	
	SDL_DestroyMutex(mTCrit); // DESTROY
}

// Set up thread, put it in a waiting state.
void MasterController::init() {

	// Create critical section
	mTCrit = SDL_CreateMutex();
	if (mTCrit == NULL) {
		fprintf(stderr, "Could not create SDL_Mutex: %s\n", SDL_GetError());
		return;
	}

	// Create the start semaphore
	mStartSem = SDL_CreateSemaphore(0);
	if (mStartSem == NULL) {
		fprintf(stderr, "Could not create SDL_Sem: %s\n", SDL_GetError());
		return;
	}

	// Create the mastercontroller thread.
	SDL_Thread *mThread = SDL_CreateThread(MasterController::_startMCThread, "Envy MasterController", this);
	if (mThread == NULL) {
		fprintf(stderr, "SDL_CreateThread failed: %s\n", SDL_GetError());
		return;
	}

	// Init successful
	fprintf(stderr, "Master Controller Initialized...\n");
	mInitialized = true;
}

// Kick off the thread.
void MasterController::run() {
	fprintf(stderr, "Master Controller Executing...\n");
	SDL_SemPost(mStartSem);
}

// Do not call this externally
void MasterController::_execute() {
	SDL_SemWait(mStartSem);
	fprintf(stderr, "Master controller out of wait state.\n");
}

// Add a frame to the frame queue (thread-safe).
void MasterController::addFrame(Frame *newFrame) {
	this->lock();
	mFrameQueue.push(newFrame);
	this->unlock();
}

// Threading utility functions
void MasterController::lock() {
	SDL_LockMutex(mTCrit);
}

void MasterController::unlock() {
	SDL_UnlockMutex(mTCrit);
}