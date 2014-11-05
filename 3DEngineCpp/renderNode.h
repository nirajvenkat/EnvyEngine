// renderNode.h
//
// The Master Controller-side representation of a hardware rendering node. The hardware rendering node is responsible
// for rendering specific requests made by the master controller.

#ifndef RENDERNODE_H
#define RENDERNODE_H

#define NODE_LATENCY_WINDOW	15

class RenderNode {
public:
	RenderNode(unsigned int number);
	virtual ~RenderNode();

	enum Status { // Need more of these for when we implement world-loading.
		READY,
		BUSY,
		FAILURE,
		RECEIVED_DATA
	};

	void assignTask(class RenderTask *task);
	void clearTask();
	void receiveResponse();

	int getNumber();
	double getLastLatency();
	void refreshRates();

private:

	void updateResponseTime();

	int mNumber;
	class RenderTask *mCurrentTask;

	// TODO: NETWORK RELATED ATTRIBUTES

	// Node stats
	float mCommandRate;		// Commands per second given by Master Controller to this node.
	float mResponseRate;    // Average response rate. This should be similar to mCommandRate for a healthy node.
	double mLastAssignTime; // Last assigned command time (using the Time::GetTime() function which wraps to QueryPerformanceFrequency.
	float mLastLatency;		// The last (individual) response rate.
	int mCurSample;			// The current sample in the mLatencySamples array, used for computing a moving average.
	float *mLatencySamples;	// Array of response times, used for computing a moving average.

};

#endif