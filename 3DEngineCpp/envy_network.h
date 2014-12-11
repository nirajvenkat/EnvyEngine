#ifndef ENVYNETWORKH
#define ENVYNETWORKH

/*
Sam's port

commented line of code was causing crashes
but i changed stuff outside of this file and it
stopped, but so it may return later

*/
#include "ENVY_TYPES.h"
#include "ENVY_CONSTANTS.h"
#include "ENVY_PACKET.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <inttypes.h>

//Windows
#include <WinSock2.h>
#include <ws2tcpip.h>
//#pragma comment(lib,"ws2_32.lib")

#define MIN(x,y) (x < y) ? x : y
#define ORDER_LITTLE_ENDIAN 1
#define ORDER_BIG_ENDIAN 2

void htonPacket(struct pkt packet, char buffer[sizeof(struct pkt)]);
struct pkt ntohPacket(char buffer[sizeof(struct pkt)]);
void htonFloat(char *bytes, float f);
float ntohFloat(char *bytes);
int getSystemEndianness();

#endif