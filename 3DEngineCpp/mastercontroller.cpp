// mastercontroller.cpp
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#include "mastercontroller.h"

using namespace std;

MasterController::MasterController() {
	
}

MasterController::~MasterController() {

}

void MasterController::init() {

}

void MasterController::run() {

}

// Add a frame to the frame queue (thread-safe).
void MasterController::addFrame(Frame *newFrame) {
	this->lock();
	this->frameQueue.push(newFrame);
	this->unlock();
}

// Threading utility functions
void MasterController::lock() {
	EnterCriticalSection(&this->tcrit);
}

void MasterController::unlock() {
	LeaveCriticalSection(&this->tcrit);
}