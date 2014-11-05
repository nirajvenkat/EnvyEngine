#include "envy_network.h"

#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

#define MAXBUF 128
#define MAX_NODES 5

void *UDPHandler(void* arg);

struct Node
{
	int sock;
	struct sockaddr_in addr;
};

int num_nodes = 0;
struct Node nodes[MAX_NODES];
pthread_t udp_thread;


int main()
{
	int i;
	for(i = 0; i < MAX_NODES; i++)
	{
		struct Node tmp;
		nodes[i] = tmp;
	}

	pthread_create(&udp_thread, NULL, &UDPHandler, NULL);
	printf("Command format: 'NODE_ID COMMAND'\n('HELP' for command list)\n\n");
	
	int finished = 0;
	char input[128];
	int node_id;
	char command[128];
	char buffer[MAXBUF];

	while(!finished)
	{
		printf(">");
		fgets(input, sizeof(input), stdin);
		sscanf(input, "%d %s", &node_id, command);
		struct pkt send_packet, recv_packet;

		if(!strcmp(command, "PING"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			send_packet.header.timestamp = 0;
			int tmp = htonl(CMD_PING);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			htonPacket(send_packet, buffer);
			sendto(nodes[node_id-1].sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&nodes[node_id-1].addr, sizeof(nodes[node_id-1].addr));
			recvfrom(nodes[node_id-1].sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&nodes[node_id-1].addr, sizeof(nodes[node_id-1].addr));
			recv_packet = ntohPacket(buffer);
			if(recv_packet.header.pkt_type == PKT_TYPE_STATUS && recv_packet.header.status == STATUS_KEEP_ALIVE)
			{
				printf("\tReceived KEEP_ALIVE from Node %d\n", node_id);
			}

		}
		else if(!strcmp(command, "UNREGISTER"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			send_packet.header.timestamp = 0;
			int tmp = htonl(CMD_UNREGISTER);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			htonPacket(send_packet, buffer);
			sendto(nodes[node_id-1].sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&nodes[node_id-1].addr, sizeof(nodes[node_id-1].addr));
			recvfrom(nodes[node_id-1].sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&nodes[node_id-1].addr, sizeof(nodes[node_id-1].addr));
			recv_packet = ntohPacket(buffer);
			if(recv_packet.header.pkt_type == PKT_TYPE_STATUS && recv_packet.header.status == STATUS_OK)
			{
				printf("\tReceived OK from Node %d\n", node_id);
			}
		}
		else if(!strcmp(command, "RESTART"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			send_packet.header.timestamp = 0;
			int tmp = htonl(CMD_RESTART);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			htonPacket(send_packet, buffer);
			sendto(nodes[node_id-1].sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&nodes[node_id-1].addr, sizeof(nodes[node_id-1].addr));
			recvfrom(nodes[node_id-1].sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&nodes[node_id-1].addr, sizeof(nodes[node_id-1].addr));
			recv_packet = ntohPacket(buffer);
			if(recv_packet.header.pkt_type == PKT_TYPE_STATUS && recv_packet.header.status == STATUS_SHUTTING_DOWN)
			{
				printf("\tReceived SHUTTING_DOWN from Node %d\n", node_id);
			}
		}
		else if(!strcmp(command, "SHUTDOWN"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			send_packet.header.timestamp = 0;
			int tmp = htonl(CMD_SHUTDOWN);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			htonPacket(send_packet, buffer);
			sendto(nodes[node_id-1].sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&nodes[node_id-1].addr, sizeof(nodes[node_id-1].addr));
			recvfrom(nodes[node_id-1].sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&nodes[node_id-1].addr, sizeof(nodes[node_id-1].addr));
			recv_packet = ntohPacket(buffer);
			if(recv_packet.header.pkt_type == PKT_TYPE_STATUS && recv_packet.header.status == STATUS_SHUTTING_DOWN)
			{
				printf("\tReceived SHUTTING_DOWN from Node %d\n", node_id);
			}
		}
		else if(!strcmp(command, "HELP"))
		{
			printf("Available commands: PING, UNREGISTER, RESTART, SHUTDOWN\n");
		}
	}
	
	pthread_join(udp_thread, NULL);
}

void *UDPHandler(void *args)
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
  //printf("Bind Status = %d\n", status);

  status = getsockname(sock, (struct sockaddr *)&sock_in, &sinlen);
  //printf("Sock port %d\n",htons(sock_in.sin_port));

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
		//fprintf(stdout, "-> %s reports successful boot! We need to register a new node...\n", addr);
		
		int cli_sock;
		struct sockaddr_in masteraddr, cliaddr;
		cli_sock = socket(AF_INET, SOCK_STREAM, 0);
		memset(&masteraddr, 0, sizeof(masteraddr));
		memset(&cliaddr, 0, sizeof(cliaddr));
		masteraddr.sin_family = AF_INET;
		masteraddr.sin_addr.s_addr = inet_addr(addr);
		masteraddr.sin_port = htons(TCP_PORT);
		connect(cli_sock, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
		//fprintf(stdout, "\tConnected with client on port %d\n", TCP_PORT);
		
		packet.header.pkt_type = PKT_TYPE_STATUS;
		packet.header.status = STATUS_OK;
		packet.header.p_length = sizeof(num_nodes);
		packet.header.timestamp = 0;
		int tmp = htonl(++num_nodes);
		memcpy(packet.payload.data, &tmp, sizeof(tmp));
		htonPacket(packet, buffer);
		//fprintf(stdout, "\tAssigning new node ID %d. Sending response...\n", num_nodes);
		sendto(cli_sock, buffer, sizeof(struct pkt), 0, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
		
		nodes[num_nodes-1].sock = cli_sock;
		nodes[num_nodes-1].addr = masteraddr;

	}
  }

  shutdown(sock, 2);
  close(sock);
}
