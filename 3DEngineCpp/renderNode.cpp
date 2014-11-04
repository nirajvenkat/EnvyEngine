// renderNode.cpp
//
// The Master Controller-side representation of a hardware rendering node. The hardware rendering node is responsible
// for rendering specific requests made by the master controller.

#include "renderNode.h"
#include "renderTask.h"

RenderNode::RenderNode(int number) 
{
	this->number = number;
}

void RenderNode::assignTask(class RenderTask *task);
int getNumber();
void refreshRates();