// simnodetests.cpp
//
// Specific testing scenarios for the simulated nodes, for purposes of load balancing.

#include "mastercontroller.h"
#include "simnodetests.h"
#include "renderNode.h"
#include "simulatednode.h"

void RunSimNodeTest(SimNodeTest testFunc, MasterController *mc)
{
	testFunc(mc);
}

// Specific unit tests for the load balancing function.
void testSimNodes4Equal(class MasterController *mc) {
	RenderNode *rn1 = new RenderNode(1);
	RenderNode *rn2 = new RenderNode(2);
	RenderNode *rn3 = new RenderNode(3);
	RenderNode *rn4 = new RenderNode(4);

	rn1->setupSimNode(10, 10, 0);
	rn2->setupSimNode(10, 10, 0);
	rn3->setupSimNode(10, 10, 0);
	rn4->setupSimNode(10, 10, 0);

	mc->addNode(rn1);
	mc->addNode(rn2);
	mc->addNode(rn3);
	mc->addNode(rn4);
}
void testSimNodes4Bad1(class MasterController *mc) {
	RenderNode *rn1 = new RenderNode(1);
	RenderNode *rn2 = new RenderNode(2);
	RenderNode *rn3 = new RenderNode(3);
	RenderNode *rn4 = new RenderNode(4);

	rn1->setupSimNode(10, 11, 0);
	rn2->setupSimNode(13, 20, 0);
	rn4->setupSimNode(4, 7, 0);
	rn3->setupSimNode(140, 170, 0);

	mc->addNode(rn1);
	mc->addNode(rn2);
	mc->addNode(rn3);
	mc->addNode(rn4);
}
void testSimNodes4Drop(class MasterController *mc) {
	RenderNode *rn1 = new RenderNode(1);
	RenderNode *rn2 = new RenderNode(2);
	RenderNode *rn3 = new RenderNode(3);
	RenderNode *rn4 = new RenderNode(4);

	rn1->setupSimNode(10, 10, 0);
	rn2->setupSimNode(10, 10, 0);
	rn3->setupSimNode(10, 10, 200);
	rn4->setupSimNode(10, 10, 0);

	mc->addNode(rn1);
	mc->addNode(rn2);
	mc->addNode(rn3);
	mc->addNode(rn4);
}