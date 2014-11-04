// renderNode.h
//
// The Master Controller-side representation of a hardware rendering node. The hardware rendering node is responsible
// for rendering specific requests made by the master controller.

#ifndef RENDERNODE_H
#define RENDERNODE_H

class RenderNode {
public:
	RenderNode(int number);

	void assignTask(class RenderTask *task);
	int getNumber();
	void refreshRates();

private:

	int number;
	int commandRate;
	int responseRate;
	unsigned long lastCommandTime;
	unsigned long lastResponseTime;
};











#endif