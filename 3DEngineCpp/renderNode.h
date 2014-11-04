// renderNode.h
//
// The Master Controller-side representation of a hardware rendering node. The hardware rendering node is responsible
// for rendering specific requests made by the master controller.

#ifndef RENDERNODE_H
#define RENDERNODE_H

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
	void refreshRates();

private:

	int number;
	class RenderTask *currentTask;

	// TODO: NETWORK RELATED ATTRIBUTES

	// Node stats
	int commandRate;
	int responseRate;
	double lastAssignTime;
	double lastLatency;
};

#endif