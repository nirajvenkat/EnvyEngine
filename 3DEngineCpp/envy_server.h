#include "ENVY_PACKET.h"

void htonPacket(struct pkt packet, char buffer[sizeof(struct pkt)]);
DWORD WINAPI TCPHandler(void* arg);
int nodeMain();
