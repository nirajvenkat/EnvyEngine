#include "envy_network.h"

#include <sys/types.h>
#include <stdio.h>

#define MAXBUF 65536
int num_nodes = 0;

int main()
{
  int sock, status, buflen;
  unsigned sinlen;
  char buffer[MAXBUF];
  struct sockaddr_in sock_in;
  int yes = 1;

  sinlen = sizeof(struct sockaddr_in);
  memset(&sock_in, 0, sinlen);

  sock = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
  sock_in.sin_port = htons(UDP_PORT);
  sock_in.sin_family = PF_INET;

  status = bind(sock, (struct sockaddr *)&sock_in, sinlen);
  printf("Bind Status = %d\n", status);

  status = getsockname(sock, (struct sockaddr *)&sock_in, &sinlen);
  printf("Sock port %d\n",htons(sock_in.sin_port));

  buflen = MAXBUF;
  memset(buffer, 1, buflen);
  while(1)
  {
  	status = recvfrom(sock, buffer, buflen, 0, (struct sockaddr *)&sock_in, &sinlen);
	struct pkt packet = ntohPacket(buffer);
	//fprintf(stdout, "Received Packet:\n\tTYPE: %" PRIu16 "\n\tNODE ID: %" PRIu16 "\n\tSTATUS: %" PRIu16 "\n\tPAYLOAD LENGTH: %" PRIu16 "\n\tTIMESTAMP: %" PRIu32 "\n", packet.header.pkt_type, packet.header.node_id, packet.header.status, packet.header.p_length, packet.header.timestamp);
	
	if(packet.header.pkt_type == PKT_TYPE_STATUS && packet.header.status == STATUS_BOOT_OK)
	{
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &sock_in.sin_addr, addr, sizeof(addr));
		fprintf(stdout, "-> %s reports successful boot! We need to register a new node...\n", addr);
		
		int cli_sock;
		struct sockaddr_in masteraddr, cliaddr;
		cli_sock = socket(AF_INET, SOCK_STREAM, 0);
		memset(&masteraddr, 0, sizeof(masteraddr));
		memset(&cliaddr, 0, sizeof(cliaddr));
		masteraddr.sin_family = AF_INET;
		masteraddr.sin_addr.s_addr = inet_addr(addr);
		masteraddr.sin_port = htons(TCP_PORT);
		connect(cli_sock, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
		fprintf(stdout, "\tConnected with client on port %d\n", TCP_PORT);
		
		packet.header.pkt_type = PKT_TYPE_STATUS;
		packet.header.status = STATUS_OK;
		packet.header.p_length = sizeof(num_nodes);
		packet.header.timestamp = 0;
		int tmp = htonl(++num_nodes);
		memcpy(packet.payload.data, &tmp, sizeof(tmp));
		htonPacket(packet, buffer);
		fprintf(stdout, "\tAssigning new node ID %d. Sending response...\n", num_nodes);
		sendto(cli_sock, buffer, sizeof(struct pkt), 0, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
		close(cli_sock);
	}
  }

  shutdown(sock, 2);
  close(sock);
}
