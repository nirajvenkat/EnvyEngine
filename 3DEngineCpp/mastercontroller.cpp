// mastercontroller.cpp
//
// Assembles frames from networked nodes and generates render tasks. This is the heart
// of the distributed portion of Envy.

#include "mastercontroller.h"
#include "renderer.h"
#include "renderNode.h"
#include "envy_network.h"
#include "windows.h"
#include <set>
#include <map>
#include <queue>
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

// TODO: Network code to wait for incoming real hardware node connections.

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

	disableRegistration();

	SDL_DestroyMutex(mTCrit); // DESTROY
}

// Entry point for a new MC thread.
int MasterController::_startMCThread(void* mc) {
	((MasterController*)mc)->_execute();
	return 0;
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

	// Create renderer
	mRenderer = new Renderer();
	mRenderer->initOutputWindow(640, 360, "Envy Master Controller");

	enableRegstration();

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

	unsigned long last = 0;
	int pause = 1000 / mFrameRateMax;
	Frame *curFrame;

	SDL_SemWait(mStartSem);
	fprintf(stderr, "Master controller out of wait state.\n");

	// Inner loop
	while (true) {
		lock();
		if (!mFrameQueue.empty()) {

			curFrame = mFrameQueue.top();
			mFrameQueue.pop();

			fprintf(stderr, "MasterController: Rendering frame %d\n", curFrame->getModelTime());
			mRenderer->renderFrame(curFrame);
			delete(curFrame);
		}
		unlock();

		Sleep(pause);
	}
}
// Add a frame to the frame queue (thread-safe).
void MasterController::addFrame(Frame *newFrame) {
	this->lock();
	mFrameQueue.push(newFrame);
	this->unlock();
}

// Add a render node
void MasterController::addNode() {
	// Insert node into node mapping. Give it an ID.
}

// Threading utility functions
void MasterController::lock() {
	SDL_LockMutex(mTCrit);
}

void MasterController::unlock() {
	SDL_UnlockMutex(mTCrit);
}

void MasterController:enableRegistration(){//creates thread to respond to registration broadcasts
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	broadCastListenerHandle = CreateThread(0, 0, registerThread, NULL, 0, &broadCastListenerID);
}

void MasterController::disableRegistration(){//cleans up thread space and closes socket
	CloseHandle(broadCastListenerHandle);
	closesocket(broadCastSocket);
	int n = 0;
	//destroy threads
	for (std::set<HANDLE>::iterator i = threads.begin(); i != threads.end(); i++){
		CloseHandle(*i);
		++n;
	}
	printf("destroyed %d threads\n", n);
	n = 0;
	//close all sockets
	for (std::set<SOCKET>::iterator i = socks.begin(); i != socks.end(); i++){
		closesocket(*i);
		++n;
	}
	printf("closed %d sockets\n", n);
	WSACleanup();
}

DWORD WINAPI MasterController::registerThread(LPVOID param){
	struct sockaddr_in mc, node;
	int slen;
	SOCKET temp;
	tSock = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	broadCastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	slen = sizeof(mc);

	mc.sin_family = AF_INET;
	mc.sin_addr.s_addr = INADDR_ANY;
	mc.sin_port = htons(UDP_PORT);

	bind(broadCastSocket, (struct sockaddr*)&mc, slen);
	bind(tSock, (const struct sockaddr*)&mc, sizeof(mc));
	listen(tSock, 1);//1 : 1 , thread : node

	pkt p;
	p.header.pkt_type = PKT_TYPE_STATUS;
	p.header.status = STATUS_BOOT_OK;
	p.header.p_length = 0;
	p.header.timestamp = 0;
	char buffer[sizeof(pkt)];
	//htonPacket(p, buffer);


	while (true){
		printf("starting loop\n");
		recvfrom(broadCastSocket, buffer, sizeof(pkt), 0, (struct sockaddr*)&node, &slen);//assumes all broadcasts are seeking registration
		printf("assigning ID: %d\n", nextID);
		printf("to %s : %d\n\n", inet_ntoa(node.sin_addr), ntohs(node.sin_port));

		//assign ID to node
		*(p.payload.data) = nextID++;

		sendto(broadCastSocket, buffer, sizeof(pkt), 0, (struct sockaddr*)&node, slen);

		//create new connection
		temp = accept(tSock, NULL, NULL);

		//create new thread
		DWORD id;
		HANDLE thread = CreateThread(0, 0, responseFunnel, &temp, 0, &id);

		//add stuff to sets
		socks.insert(temp);
		threads.insert(thread);
		rNodes.insert(std::pair<HANDLE, RenderNode*>(thread, new RenderNode(nextID - 1)));
	}

}

DWORD WINAPI MasterController::responseFunnel(LPVOID param){//create tcp connection and listen for responses
		SOCKET nodeStream = *(SOCKET*)param;
		int node = rNodes.at(GetCurrentThread())->getNumber();
		pkt p;
		char buffer[sizeof(pkt)];
		while (true){
			recv(nodeStream, buffer, sizeof(pkt), 0);
			p = ntohPacket(buffer);
			if (p.header.pkt_type == PKT_TYPE_STATUS){
				if (p.header.status == STATUS_OK){
					printf("Received STATUS_OK from node: %d", node);
				}
				else if (p.header.status == STATUS_BOOT_OK){
					printf("Received STATUS_BOOT_OK from node: %d", node);
				}
				else if (p.header.status == STATUS_SHUTTING_DOWN){
					printf("Received STATUS_SHUTTING_DOWN from node: %d", node);
				}
				else if (p.header.status == STATUS_KEEP_ALIVE){
					printf("Received STATUS_KEEP_ALIVE from node: %d", node);
				}
				else if (p.header.status == STATUS_STATISTIC){
					printf("Received STATUS_STATISTIC from node: %d", node);
				}
				else{
					printf("Invalid Status %d\n", p.header.status);
				}

			}
			else if (p.header.pkt_type == PKT_TYPE_CMD){//probably wont get any of these
				if (*(int*)p.payload.data == CMD_PING){
					printf("Received PING from node: %d", node);
				}
				else if (*(int*)p.payload.data == CMD_UNREGISTER){
					printf("Received UNREGISTER from node: %d", node);
				}
				else if (*(int*)p.payload.data == CMD_RESTART){
					printf("Received RESTART from node: %d", node);
				}
				else if (*(int*)p.payload.data == CMD_SHUTDOWN){
					printf("Received SHUTDOWN from node: %d", node);
				}
			}
			else if (p.header.pkt_type == PKT_TYPE_TASK){//responses
				rNodes.at(GetCurrentThread())->receiveResponse();
				printf("Received TASK from socket: %d", nodeStream);
				//change status
				//do stuff with the RenderTask
			}
		}
	}
}
