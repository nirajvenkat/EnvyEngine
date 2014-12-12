#pragma once 
#include "envy_network.h"
#include <windows.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "envy_mc_driver.h"
#include "mastercontroller.h"
#include "renderNode.h"
#include<set>
#include<map>

using namespace std;

MasterController *gmc;

//masterConroller networking fields
int nextID = 1;//used to assign IDs' to threads
std::set<int> addresses;//ip addresses	-spam Filter

SOCKET broadCastSocket;// 
HANDLE broadCastListenerHandle;
DWORD broadCastListenerID;


//for testing
//production wont include these
std::map<int, SOCKET> idtosock; 

//testing
#define MAXBUF 1024
SOCKET IDtoSocket(int id){
	printf("ID: -> %d\n",id,idtosock[id]);
	return idtosock[id];
}

//testing
void sendCommand(){
	printf("Command format: 'NODE_ID COMMAND'\n('HELP' for command list)\n\n");
	int finished = 0;
	char input[128];
	int node_id;
	SOCKET sock;
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

		sock=IDtoSocket(node_id);

		if (!strcmp(command, "PING"))
		{
			send_packet.header.pkt_type = PKT_TYPE_CMD;
			send_packet.header.status = STATUS_OK;
			send_packet.header.p_length = sizeof(int);
			int tmp = htons(CMD_PING);
			memcpy(send_packet.payload.data, &tmp, sizeof(tmp));
			memset(&send_packet.header.timestamp, 0, sizeof(float));

			SYSTEMTIME tv;
			GetSystemTime(&tv);
			double send_time = tv.wSecond * 1000.0 + tv.wMilliseconds;// possible /1000

			//htonPacket(send_packet, buffer);
			//sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			int r;
			r=send(sock, (char*)&send_packet, sizeof(send_packet), 0);
			if(r<=0)
				printf("send ERROR %d\n",WSAGetLastError());
			
			
			buffer[0] = -1;

			//recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			r=recv(sock, buffer, sizeof(recv_packet), 0);
			if(r<=0)
				printf("rec ERROR %d\n",WSAGetLastError());
			
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
			//sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			int r;
			r=send(sock, buffer, sizeof(send_packet), 0);
			if(r<=0)
				printf("send ERROR %d\n",WSAGetLastError());
			else
				printf("sent %d bytes\n",r);
			
			buffer[0] = -1;



	
			//printf("REMOVING %s\n\n", inet_ntoa(*(struct in_addr*)&add));
			//addresses.erase(add);
			
			//recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			r=recv(sock, buffer, sizeof(recv_packet), 0);
			if(r<=0)
				printf("rec ERROR %d\n",WSAGetLastError());
			else
				printf("received %d bytes\n",r);

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
					//closesocket(nodes[node_id - 1].sock);
					closesocket(sock);
				}
			}
			else
			{
				printf("\tResponse from Node %d timed out...(Timeout is %d ms)\n", node_id, RECV_TIMEOUT);
			}
		}
		else if (!strcmp(command, "RESTART"))
		{}
		else if (!strcmp(command, "SHUTDOWN"))
		{}
	}
}

DWORD WINAPI responseFunnel(LPVOID param){
	//create tcp connection and listen for responses
	RenderNode * renderNode= (RenderNode*)param;
	while (true){
		renderNode->receiveResponse();
	}
	return 0;
}

boolean newAddress(_int32* addr){
	//inet_ntoa(node.sin_addr)
	//fprintf(stdout, "CHECKING ADDRESS %s\n", inet_ntoa(*(in_addr*)addr));
	std::set<_int32>::iterator it = addresses.find(*addr);
	if (it != addresses.end())
		return false;
	return true;
}

void sendAck(SOCKET sock){
	struct pkt packet;
	char buffer[MAXBUF];
	int cli_sock;
	struct sockaddr_in masteraddr, cliaddr;
	int tv = RECV_TIMEOUT;

	packet.header.pkt_type = PKT_TYPE_STATUS;
	packet.header.status = STATUS_OK;
	packet.header.p_length = sizeof(_int32);
	memset(&packet.header.timestamp, 0, sizeof(float));

	int tmp = nextID-1;
	memcpy(packet.payload.data, &tmp, sizeof(tmp));
	//htonPacket(packet, buffer);
	//fprintf(stdout, "\t* Assigning new node ID %d. Sending response...\n", num_nodes);
	//int sent = sendto(cli_sock, buffer, sizeof(struct pkt), 0, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
	printf("sending ACK....");
	int r =send(sock, (char*)&packet, sizeof(struct pkt), 0);
	printf("sent %d bytes\n",r);
	printf("Error %d\n",WSAGetLastError());
}

DWORD WINAPI registerThread(LPVOID param){
	struct sockaddr_in mc, node;
	int slen;
	SOCKET temp;

	broadCastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	slen = sizeof(mc);

	mc.sin_family = AF_INET;
	mc.sin_addr.s_addr = INADDR_ANY;
	mc.sin_port = htons(UDP_PORT);

	bind(broadCastSocket, (struct sockaddr*)&mc, slen);

	pkt p;
	p.header.pkt_type = PKT_TYPE_STATUS;
	p.header.status = STATUS_BOOT_OK;
	p.header.p_length = 0;
	memset(p.header.timestamp, 0, sizeof(p.header.timestamp));
	char buffer[sizeof(pkt)];

	fprintf(stdout,"Master Controller listening for new nodes\n");
	while (true){
		recvfrom(broadCastSocket, buffer, sizeof(pkt), 0, (struct sockaddr*)&node, &slen);//assumes all broadcasts are seeking registration
		if (!newAddress((_int32*)&node.sin_addr.s_addr)){
			fprintf(stdout,"Caught garbage broadcast\n");
			continue;
		}
		printf("assigning ID: %d ", nextID);
		printf("to %s : %d\n\n", inet_ntoa(node.sin_addr), ntohs(node.sin_port));

		//assign ID to node
		*(p.payload.data) = nextID++;
		
		//create new connection
		temp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(temp<=0)
			printf("socket ERROR %d\n", WSAGetLastError());

		node.sin_port = htons(TCP_PORT);
		int res = connect(temp, (const sockaddr*)&node, sizeof(node));
		if (res < 0)
			printf("connect ERROR %d\n", WSAGetLastError());
		//else
			//printf("Successful connection on SOCKET !\n");
		sendAck(temp);
		
		RenderNode *newNode = new RenderNode(nextID-1);
		newNode->setNodeInAddr(&(node.sin_addr));
		newNode->setSocket(temp);
		
		//create new thread
		DWORD id;
		HANDLE thread;
		thread = CreateThread(0, 0, responseFunnel, newNode, 0, &id);

		newNode->setConnHandle(thread);

		gmc->lock();
		gmc->addNode(newNode);
		gmc->unlock();
		

		//add stuff to sets
		addresses.insert(node.sin_addr.s_addr);
		
		//testing structs got depreciated
		//stoaddr[*temp]=node.sin_addr.s_addr;
		//idtosock[nextID-1]=*temp;
	}
}

void enableRegistration(){//creates thread to respond to registration broadcasts
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	broadCastListenerHandle = CreateThread(0, 0, registerThread, NULL, 0, &broadCastListenerID);
}

void disableRegistration(){//cleans up thread space and closes socket
	CloseHandle(broadCastListenerHandle);
	closesocket(broadCastSocket);
	WSACleanup();
}

/*
int main(int argc, char** argv){
	enableRegistration();
	sendCommand();
	disableRegistration();
}*/