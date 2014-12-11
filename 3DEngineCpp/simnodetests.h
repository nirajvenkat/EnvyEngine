// simnodetests.h
//
// Specific testing scenarios for the simulated nodes, for purposes of load balancing.

typedef void(*SimNodeTest)(class MasterController*);

void RunSimNodeTest(SimNodeTest testFunc, class MasterController *mc);

// Specific test functions
void testSimNodes4Equal(class MasterController *mc);
void testSimNodes3Bad1(class MasterController *mc);
void testSimNodes4Drop(class MasterController *mc);
