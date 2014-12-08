/*
porting Sam's code

no longer used

check envy_mc_driver.cpp
*/
#include "envy_network.h"

#include <sys/types.h>
#include <stdio.h>
#include <time.h>

#include <Windows.h>

#define MAXBUF 1024
#define MAX_NODES 5

DWORD WINAPI UDPHandler(void* arg);

struct Node
{
	int sock;
	struct sockaddr_in addr;
};

int num_nodes = 0;
struct Node nodes[MAX_NODES];

//pthread_t udp_thread;
HANDLE udp_thread;
DWORD udp_thead_id;


int main(int argc, char** argv)
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	int i;
	for (i = 0; i < MAX_NODES; i++)
	{
		struct Node tmp;
		nodes[i] = tmp;
	}

	//pthread_create(&udp_thread, NULL, &UDPHandler, NULL);
	udp_thread = CreateThread(0, 0, UDPHandler, NULL, 0, &udp_thead_id);

	printf("Command format: 'NODE_ID COMMAND'\n('HELP' for command list)\n\n");

	int finished = 0;
	char input[128];
	int node_id;
	char command[128];
	char buffer[MAXBUF];
	struct pkt send_packet, recv_packet;
	struct timeval tv;

	while (!finished)
	{
		//printf(">");
		node_id = 0;
		memset(buffer, 0, sizeof(buffer));
		memset(command, 0, sizeof(command));
		memset(input, 0, sizeof(input));
		memset(&send_packet, 0, sizeof(send_packet));
		memset(&recv_packet, 0, sizeof(send_packet));
		fgets(input, sizeof(input), stdin);
		sscanf(input, "%d %s", &node_id, command);

		if (!strcmp(command, "PING"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			int tmp = htons(CMD_PING);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			//send_packet.header.timestamp = 0;
			memset(&send_packet.header.timestamp, 0, sizeof(float));

			//gettimeofday(&tv, NULL);
			SYSTEMTIME tv;
			GetSystemTime(&tv);
			double send_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000
			
			htonPacket(send_packet, buffer);
			sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			buffer[0] = -1;
			
			//stupid change
			//recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			int size = sizeof(nodes[node_id - 1].addr);
			recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			
			if (buffer[0] != -1)
			{
				recv_packet = ntohPacket(buffer);
				if (recv_packet.header.pkt_type == PKT_TYPE_STATUS && recv_packet.header.status == STATUS_KEEP_ALIVE)
				{
					printf("\tReceived KEEP_ALIVE from Node %d\n", node_id);
					//gettimeofday(&tv, NULL);
					GetSystemTime(&tv);
					double recv_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000

					printf("\tLATENCY: %f ms\n", recv_time - send_time);
				}
			}
			else
			{
				printf("\tResponse from Node %d timed out...(Timeout is %d seconds)\n", node_id, RECV_TIMEOUT);
			}

		}
		else if (!strcmp(command, "UNREGISTER"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			//send_packet.header.timestamp = 0;
			memset(&send_packet.header.timestamp, 0, sizeof(float));

			//gettimeofday(&tv, NULL);
			SYSTEMTIME tv;
			GetSystemTime(&tv);
			double send_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000
			
			
			int tmp = htonl(CMD_UNREGISTER);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			htonPacket(send_packet, buffer);
			sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			buffer[0] = -1;
			//
			int size = sizeof(nodes[node_id - 1].addr);
			recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			if (buffer[0] != -1)
			{
				recv_packet = ntohPacket(buffer);
				if (recv_packet.header.pkt_type == PKT_TYPE_STATUS && recv_packet.header.status == STATUS_OK)
				{
					printf("\tReceived OK from Node %d\n", node_id);
					//gettimeofday(&tv, NULL);
					GetSystemTime(&tv);

					//double recv_time = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
					double recv_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000
					printf("\tLATENCY: %f ms\n", recv_time - send_time);
					closesocket(nodes[node_id - 1].sock);
				}
			}
			else
			{
				printf("\tResponse from Node %d timed out...(Timeout is %d seconds)\n", node_id, RECV_TIMEOUT);
			}
		}
		else if (!strcmp(command, "RESTART"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			//send_packet.header.timestamp = 0;
			memset(&send_packet.header.timestamp, 0, sizeof(float));

			//gettimeofday(&tv, NULL);
			SYSTEMTIME tv;
			GetSystemTime(&tv);
			double send_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000

			int tmp = htonl(CMD_RESTART);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			htonPacket(send_packet, buffer);
			sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			buffer[0] = -1;
			//
			int size = sizeof(nodes[node_id - 1].addr);
			recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			
			if (buffer[0] != -1)
			{
				recv_packet = ntohPacket(buffer);
				if (recv_packet.header.pkt_type == PKT_TYPE_STATUS && recv_packet.header.status == STATUS_SHUTTING_DOWN)
				{
					printf("\tReceived SHUTTING_DOWN from Node %d\n", node_id);
					//gettimeofday(&tv, NULL);
					GetSystemTime(&tv);

					//double recv_time = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
					double recv_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000

					printf("\tLATENCY: %f ms\n", recv_time - send_time);
				}
			}
			else
			{
				printf("\tResponse from Node %d timed out...(Timeout is %d seconds)\n", node_id, RECV_TIMEOUT);
			}
		}
		else if (!strcmp(command, "SHUTDOWN"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			//send_packet.header.timestamp = 0;
			memset(&send_packet.header.timestamp, 0, sizeof(float));

			//gettimeofday(&tv, NULL);
			SYSTEMTIME tv;
			GetSystemTime(&tv);
			double send_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000

			int tmp = htonl(CMD_SHUTDOWN);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			htonPacket(send_packet, buffer);
			sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			buffer[0] = -1;
			int size = sizeof(nodes[node_id - 1].addr);
			recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			if (buffer[0] != -1)
			{
				recv_packet = ntohPacket(buffer);
				if (recv_packet.header.pkt_type == PKT_TYPE_STATUS && recv_packet.header.status == STATUS_SHUTTING_DOWN)
				{
					printf("\tReceived SHUTTING_DOWN from Node %d\n", node_id);
					//gettimeofday(&tv, NULL);
					GetSystemTime(&tv);

					//double recv_time = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
					double recv_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000
					
					printf("\tLATENCY: %f ms\n", recv_time - send_time);
				}
			}
			else
			{
				printf("\tResponse from Node %d timed out...(Timeout is %d seconds)\n", node_id, RECV_TIMEOUT);
			}
		}
		else if (!strcmp(command, "UNAME"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			//send_packet.header.timestamp = 0;
			memset(&send_packet.header.timestamp, 0, sizeof(float));

			//gettimeofday(&tv, NULL);
			SYSTEMTIME tv;
			GetSystemTime(&tv);
			//double send_time = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
			double send_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000

			int tmp = htonl(CMD_UNAME);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			htonPacket(send_packet, buffer);
			sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			buffer[0] = -1;
			int size = sizeof(nodes[node_id - 1].addr);
			recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			if (buffer[0] != -1)
			{
				recv_packet = ntohPacket(buffer);
				if (recv_packet.header.pkt_type == PKT_TYPE_STATUS && recv_packet.header.status == STATUS_OK)
				{
					printf("\tReceived UNAME from Node %d\n", node_id);
					printf("\t%s\n", recv_packet.payload.data);
					//gettimeofday(&tv, NULL);
					GetSystemTime(&tv);

					//double recv_time = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
					double recv_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000
					printf("\tLATENCY: %f ms\n", recv_time - send_time);
				}
			}
			else
			{
				printf("\tResponse from Node %d timed out...(Timeout is %d seconds)\n", node_id, RECV_TIMEOUT);
			}
		}
		else if (!strcmp(command, "HELP"))
		{
			printf("Available commands: PING, UNREGISTER, RESTART, SHUTDOWN\n");
		}
	}

	//pthread_join(udp_thread, NULL);
	CloseHandle(udp_thread);
}

DWORD WINAPI UDPHandler(void *args)
{
	int sock, status, buflen;
	unsigned sinlen;
	char buffer[MAXBUF];
	struct sockaddr_in sock_in;
	int yes = 1;

	sinlen = sizeof(struct sockaddr_in);
	memset(&sock_in, 0, sinlen);

	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_in.sin_port = htons(UDP_PORT);
	sock_in.sin_family = PF_INET;

	status = bind(sock, (struct sockaddr *)&sock_in, sinlen);
	//printf("Bind Status = %d\n", status);

	//added cast
	status = getsockname(sock, (struct sockaddr *)&sock_in, (int*)&sinlen);
	//printf("Sock port %d\n",htons(sock_in.sin_port));

	buflen = MAXBUF;
	memset(buffer, 1, buflen);
	while (1)
	{
		fprintf(stdout,"waiting for broadcasts\n");
		int s = sinlen;
		while ((status = recvfrom(sock, buffer, buflen, 0, (struct sockaddr *)&sock_in, &s)) <= 0){ fprintf(stdout, "ERROR %d\n", WSAGetLastError()); }

		fprintf(stdout, "RECEIVED PACKET and %d bytes\n",status);
		
		struct pkt packet = ntohPacket(buffer);
		//struct pkt packet = *(struct pkt*)buffer;
		//fprintf(stdout, "Received Packet:\n\tTYPE: %" PRIu16 "\n\tSTATUS: %" PRIu16 "\n\tPAYLOAD LENGTH: %" PRIu16 "\n\tTIMESTAMP: %" PRIu32 "\n", packet.header.pkt_type, packet.header.status, packet.header.p_length, packet.header.timestamp);

		if (packet.header.pkt_type == PKT_TYPE_STATUS && packet.header.status == STATUS_BOOT_OK)
		{
			fprintf(stdout,"PROCESSING PACKET\n");
			char addr[INET_ADDRSTRLEN];

			//inet_ntop(AF_INET, &sock_in.sin_addr, addr, sizeof(addr));
			InetNtopW(AF_INET, &sock_in.sin_addr, (PWSTR)addr, sizeof(addr));

			fprintf(stdout, "-> %s reports successful boot! We need to register a new node...\n", addr);

			int cli_sock;
			struct sockaddr_in masteraddr, cliaddr;
			cli_sock = socket(AF_INET, SOCK_STREAM, 0);
			int tv = RECV_TIMEOUT;
			setsockopt(cli_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
			memset(&masteraddr, 0, sizeof(masteraddr));
			memset(&cliaddr, 0, sizeof(cliaddr));
			masteraddr.sin_family = AF_INET;
			masteraddr.sin_addr.s_addr = inet_addr(addr);
			masteraddr.sin_port = htons(TCP_PORT);


			connect(cli_sock, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
			fprintf(stdout, "\t* Connected to new node on port %d\n", TCP_PORT);

			packet.header.pkt_type = PKT_TYPE_STATUS;
			packet.header.status = STATUS_OK;
			packet.header.p_length = sizeof(num_nodes);
			//send_packet.header.timestamp = 0;
			memset(&packet.header.timestamp, 0, sizeof(float));

			int tmp = htonl(++num_nodes);
			memcpy(packet.payload.data, &tmp, sizeof(tmp));
			htonPacket(packet, buffer);
			fprintf(stdout, "\t* Assigning new node ID %d. Sending response...\n", num_nodes);
			int sent=sendto(cli_sock, buffer, sizeof(struct pkt), 0, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
			if (sent <= 0){
				fprintf(stdout, "ERROR %d\n", WSAGetLastError());
			}
			nodes[num_nodes - 1].sock = cli_sock;
			nodes[num_nodes - 1].addr = masteraddr;

		}
	}

}