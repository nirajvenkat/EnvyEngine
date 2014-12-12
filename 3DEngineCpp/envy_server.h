#include "ENVY_PACKET.h"

void RenderAndSend(SOCKET client, class RenderTask *rt, class Renderer *renderer);
void htonPacket(struct pkt packet, char buffer[sizeof(struct pkt)]);
DWORD WINAPI TCPHandler(void* arg);
int nodeMain();
