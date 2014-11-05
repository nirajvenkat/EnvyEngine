// renderNode.h
//
// The Master Controller-side representation of a hardware rendering node. The hardware rendering node is responsible
// for rendering specific requests made by the master controller.

#ifndef RENDERNODE_H
#define RENDERNODE_H

#define NODE_LATENCY_WINDOW	15

class RenderNode {
public:
	RenderNode(int number);
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

	int mNumber;
	class RenderTask *mCurrentTask;

	// TODO: NETWORK RELATED ATTRIBUTES

	// Node stats
	float mCommandRate;
	float mResponseRate;
	float mLastAssignTime;
	float mLastLatency;
	float *mLatencies;
};

#endif