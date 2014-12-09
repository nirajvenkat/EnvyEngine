/*
accepts broadcasts
makes TCP connections

TODO
seperate sending requests and reveiving requests in sendCommand()
	make main send them
	each connection thread should process them and respond to the main thread

integrate with other classes

*/

#include "envy_network.h"
//#include "renderNode.h"

#include <sys/types.h>
#include <stdio.h>
#include <time.h>

#include <Windows.h>

#include<set>
#include<map>
using namespace std;

//masterConroller networking fields
std::set<HANDLE> threads;//set of handles for thread
std::set<SOCKET*> socks;//sockets
std::set<_int32> addresses;

//CHANGE this around the code
//std::map<HANDLE, RenderNode*> rNodes;//mapps from handles to RenderNodes
std::map<HANDLE, int> rNodes;

SOCKET broadCastSocket, tSock;
HANDLE broadCastListenerHandle;
DWORD broadCastListenerID;
int nextID = 1;//used to assign IDs' to threads

void enableRegistration();
void disableRegistration();
DWORD WINAPI registerThread(LPVOID);


#define MAXBUF 1024


void sendCommand(SOCKET sock){
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
			//sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			send(sock, buffer, sizeof(send_packet), 0);
			buffer[0] = -1;

			//stupid change
			//recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			//int size = sizeof(nodes[node_id - 1].addr);
			//recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			recv(sock, buffer, sizeof(recv_packet), 0);
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
			send(sock, buffer, sizeof(send_packet), 0);

			buffer[0] = -1;
			//
			//int size = sizeof(nodes[node_id - 1].addr);
			//recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			recv(sock, buffer, sizeof(recv_packet), 0);

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
			//sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			send(sock, buffer, sizeof(send_packet), 0);

			buffer[0] = -1;
			//
			//int size = sizeof(nodes[node_id - 1].addr);
			//recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			recv(sock, buffer, sizeof(recv_packet), 0);

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
			//sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			send(sock, buffer, sizeof(send_packet), 0);

			buffer[0] = -1;
			//int size = sizeof(nodes[node_id - 1].addr);
			//recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			recv(sock, buffer, sizeof(recv_packet), 0);

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
			//sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			send(sock, buffer, sizeof(send_packet), 0);

			buffer[0] = -1;
			//int size = sizeof(nodes[node_id - 1].addr);
			//recvfrom(nodes[node_id - 1].sock, buffer, sizeof(recv_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, &size);
			recv(sock, buffer, sizeof(recv_packet), 0);

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
}



DWORD WINAPI responseFunnel(LPVOID param){//create tcp connection and listen for responses
	fprintf(stdout, "NEW THREAD\n");

	SOCKET nodeStream = *(SOCKET*)param;
	//int node = rNodes.at(GetCurrentThread())->getNodeId();
	//rNodes.at(GetCurrentThread());
	int node = rNodes[GetCurrentThread()];
	pkt p;
	char buffer[sizeof(pkt)];
	fprintf(stdout,"GETTING INPUT\n");
	sendCommand(nodeStream);
	//while (true){
			
	//}
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
	//cli_sock = socket(AF_INET, SOCK_STREAM, 0);
	int tv = RECV_TIMEOUT;
	//setsockopt(cli_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
	//memset(&masteraddr, 0, sizeof(masteraddr));
	//memset(&cliaddr, 0, sizeof(cliaddr));
	//masteraddr.sin_family = AF_INET;
	//masteraddr.sin_addr.s_addr = inet_addr(addr);
	//masteraddr.sin_port = htons(TCP_PORT);


	//connect(cli_sock, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
	//fprintf(stdout, "\t* Connected to new node on port %d\n", TCP_PORT);

	packet.header.pkt_type = PKT_TYPE_STATUS;
	packet.header.status = STATUS_OK;
	packet.header.p_length = sizeof(_int32);
	//send_packet.header.timestamp = 0;
	memset(&packet.header.timestamp, 0, sizeof(float));

	int tmp = htonl(nextID-1);
	memcpy(packet.payload.data, &tmp, sizeof(tmp));
	htonPacket(packet, buffer);
	//fprintf(stdout, "\t* Assigning new node ID %d. Sending response...\n", num_nodes);
	//int sent = sendto(cli_sock, buffer, sizeof(struct pkt), 0, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
	send(sock, buffer, sizeof(struct pkt), 0);
}

DWORD WINAPI registerThread(LPVOID param){
	struct sockaddr_in mc,  
		node;
	int slen;
	SOCKET *temp;
	struct sockaddr nodeName;

	tSock = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	broadCastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	slen = sizeof(mc);

	mc.sin_family = AF_INET;
	mc.sin_addr.s_addr = INADDR_ANY;
	mc.sin_port = htons(UDP_PORT);

	bind(broadCastSocket, (struct sockaddr*)&mc, slen);
	bind(tSock, (const struct sockaddr*)&mc, sizeof(mc));
	listen(tSock, 1);//1 : 1 , thread : node

	pkt p;
	p.header.pkt_type = PKT_TYPE_STATUS;
	p.header.status = STATUS_BOOT_OK;
	p.header.p_length = 0;
	//p.header.timestamp = 0;
	memset(p.header.timestamp, 0, sizeof(p.header.timestamp));
	char buffer[sizeof(pkt)];
	//htonPacket(p, buffer);

	fprintf(stdout,"Master Controller listening for new nodes\n");
	while (true){
		recvfrom(broadCastSocket, buffer, sizeof(pkt), 0, (struct sockaddr*)&node, &slen);//assumes all broadcasts are seeking registration
		//fprintf(stdout,"BROADCAST RECEIVED\n");
		if (!newAddress((_int32*)&node.sin_addr.s_addr)){
			//fprintf(stdout,"Caught garbage broadcast\n");
			continue;
		}
		printf("assigning ID: %d\n", nextID);
		printf("to %s : %d\n\n", inet_ntoa(node.sin_addr), ntohs(node.sin_port));

		//assign ID to node
		*(p.payload.data) = nextID++;

		//old
		//sendto(broadCastSocket, buffer, sizeof(pkt), 0, (struct sockaddr*)&node, slen);
		//temp = (SOCKET*)malloc(sizeof(SOCKET));
		//*temp = accept(tSock, NULL, NULL);
		//fprintf(stdout, "Accepted TCP with SOCKET%d\n", temp);
		
		//create new connection
		temp = (SOCKET*)malloc(sizeof(SOCKET));
		*temp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(*temp<=0)
			printf("socket ERROR %d\n", WSAGetLastError());
		else
			printf("socket %d\n", temp);

		node.sin_port = htons(TCP_PORT);
		int res = connect(*temp, (const sockaddr*)&node, sizeof(node));
		if (res < 0)
			printf("connect ERROR %d\n", WSAGetLastError());
		else
			printf("Successful connection!\n");
		
		sendAck(*temp);
		
		//create new thread
		DWORD id;
		HANDLE thread;
		thread = CreateThread(0, 0, responseFunnel, &temp, 0, &id);

		//add stuff to sets
		addresses.insert(node.sin_addr.s_addr);
		socks.insert(temp);
		threads.insert(thread);
		//rNodes.insert(std::pair<HANDLE, RenderNode*>(thread, new RenderNode(nextID - 1)));
		rNodes[thread] = nextID++;
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
	int n = 0;
	//destroy threads
	for (std::set<HANDLE>::iterator i = threads.begin(); i != threads.end(); i++){
		CloseHandle(*i);
		++n;
	}
	printf("destroyed %d threads\n", n);
	n = 0;
	//close all sockets
	for (std::set<SOCKET*>::iterator i = socks.begin(); i != socks.end(); i++){
		closesocket(**i);
		free(*i);
		++n;
	}
	printf("closed %d sockets\n", n);
	WSACleanup();
}


int main(int argc, char** argv){
	enableRegistration();
	while (true);
	//getc(stdin);
	disableRegistration();
}