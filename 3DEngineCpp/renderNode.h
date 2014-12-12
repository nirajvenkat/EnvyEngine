// renderNode.h
//
// The Master Controller-side representation of a hardware rendering node. The hardware rendering node is responsible
// for rendering specific requests made by the master controller.

//#define SIMULATE testSimNodes3Bad1
//#define SIMULATE testSimNodes4Equal

#ifndef RENDERNODE_H
#define RENDERNODE_H

#define NODE_LATENCY_WINDOW	15

#include "windows.h"

class RenderNode {
public:
	RenderNode(unsigned int number);
	virtual ~RenderNode();

	enum Status { // Need more of these for when we implement world-loading.
		READY,
		BUSY,
		FAILURE,
		RECEIVED_DATA,
		LOADING_DATA // Might drop this after testing concludes
	};

	void assignTask(class RenderTask *task);
	void clearTask();
	void receiveResponse();
	RenderNode::Status getStatus();
	void setFinishedFrame(class Frame*);
	class Frame *unloadFinishedFrame();

	int getNodeId();
	void setNodeInAddr(struct in_addr *addr);
	void getNodeInAddr(struct in_addr *dest);
	void setSocket(SOCKET sock);
	double getLastLatency();
	float getAvgLatency();
	void refreshRates();
	bool renderReady();

#ifdef SIMULATE
	void setupSimNode(int minLatency, int maxLatency, int dropTime);
	//void receiveSimData(class Frame *newFrame);
	void simNullResponse();
#endif

private:

	void updateResponseTime();

	int mId;
	class RenderTask *mCurrentTask;
	Status mStatus;
	class Frame *mFinishedFrame;

#ifdef SIMULATE
	class SimulatedNode *mSimNode;
#endif

    void *mBitmap;

	// TODO: NETWORK RELATED ATTRIBUTES
	struct in_addr *mAddr;
	SOCKET mSocket;

	// Node stats
	float mCommandRate;		// Commands per second given by Master Controller to this node.
	float mResponseRate;    // Average response rate. This should be similar to mCommandRate for a healthy node.
	double mLastAssignTime; // Last assigned command time (using the Time::GetTime() function which wraps to QueryPerformanceFrequency.
	float mLastLatency;		// The last (individual) response rate.
	int mCurSample;			// The current sample in the mLatencySamples array, used for computing a moving average.
	float *mLatencySamples;	// Array of response times, used for computing a moving average.
	
};

#endif