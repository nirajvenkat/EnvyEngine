/*
accepts broadcasts
makes TCP connections

TODO
get use cases of messages

integrate with other classes

*/

#include "envy_network.h"

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "envy_mc_driver.h"
#include <Windows.h>

#include<set>
#include<map>
using namespace std;

//masterConroller networking fields
std::set<HANDLE> threads;//set of handles for thread - disableReg
std::set<SOCKET> socks;//sockets	-	disableReg
std::set<_int32> addresses;//ip addresses	-spam Filter

//CHANGE this around in the code
//std::map<HANDLE, RenderNode*> rNodes;//mapps from handles to RenderNodes
std::map<HANDLE, int> rNodesID;

std::map<SOCKET, _int32> stoaddr;//to remove addresses from spamfilter
std::map<SOCKET, int> stid;

//for testing
//production wont include these
std::map<int, SOCKET> idtosock; 

SOCKET broadCastSocket;//, tSock;
HANDLE broadCastListenerHandle;
DWORD broadCastListenerID;

int nextID = 1;//used to assign IDs' to threads

#define MAXBUF 1024

SOCKET IDtoSocket(int id){
	printf("ID: -> %d\n",id,idtosock[id]);
	return idtosock[id];
}


void getResponse(SOCKET sock){
	/*
	char buffer[sizeof(struct pkt)];
	int r = recv(sock,buffer,sizeof(buffer),0);
	if(r<=0)
		printf("ERROR %d\n",WSAGetLastError());

	struct pkt recv_packet = ntohPacket(buffer);
	int tmp;
	memcpy(&tmp, recv_packet.payload.data, sizeof(tmp));
	int node_id = ntohl(tmp);
	fprintf(stdout, "Received acknowledgement from master controller! My node ID is %d!\n", NODE_ID);


	if (recv_packet.header.pkt_type == PKT_TYPE_TASK)
			{
				//We got a task from MC....        
				switch (recv_packet.header.status) //Data for task command will be in the status field
				{
				case TASK_LOAD_WORLD:
					break;
				}
			}
			else if (recv_packet.header.pkt_type == PKT_TYPE_CMD)
			{
				//We got a command to do something...
				memcpy(&tmp, recv_packet.payload.data, sizeof(tmp)); //Data for command will be in the payload

				switch (ntohl(tmp))
				{
				case CMD_PING:
					send_packet.header.pkt_type = PKT_TYPE_STATUS;
					send_packet.header.status = STATUS_KEEP_ALIVE;
					send_packet.header.p_length = 0;

					//send_packet.header.timestamp = recv_packet.header.timestamp;
					memcpy(&send_packet.header.timestamp, &recv_packet.header.timestamp, sizeof(float));

					htonPacket(send_packet, buffer);
					status = sendto(client, buffer, sizeof(buffer), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
					fprintf(stdout, "Received PING from Master; sending back KEEP_ALIVE.\n");
					break;
				case CMD_UNREGISTER:
					send_packet.header.pkt_type = PKT_TYPE_STATUS;
					send_packet.header.status = STATUS_OK;
					send_packet.header.p_length = 0;

					//send_packet.header.timestamp = recv_packet.header.timestamp;
					memcpy(&send_packet.header.timestamp, &recv_packet.header.timestamp, sizeof(float));

					htonPacket(send_packet, buffer);
					status = sendto(client, buffer, sizeof(buffer), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
					fprintf(stdout, "Received UNREGISTER from Master. Terminating thread.\n");
					finished = true;
					NODE_ID = 0;
					//while (WaitForSingleObject(lock, INFINITE) != WAIT_OBJECT_0);
					//didACK = false;
					//ReleaseMutex(lock);
					closesocket(fd);
					break;
				}
			}
			else if (recv_packet.header.pkt_type == PKT_TYPE_STATUS)
			{
				//We got a status update or statistic...
			}
			*/
}


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

			htonPacket(send_packet, buffer);
			//sendto(nodes[node_id - 1].sock, buffer, sizeof(send_packet), 0, (struct sockaddr*)&nodes[node_id - 1].addr, sizeof(nodes[node_id - 1].addr));
			int r;
			r=send(sock, buffer, sizeof(send_packet), 0);
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



			_int32 add=stoaddr[sock];

			printf("REMOVING %s\n\n", inet_ntoa(*(struct in_addr*)&add));
			addresses.erase(add);
			
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
				printf("\tResponse from Node %d timed out...(Timeout is %d ms)\n", node_id, RECV_TIMEOUT);
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
				printf("\tResponse from Node %d timed out...(Timeout is %d ms)\n", node_id, RECV_TIMEOUT);
			}
		}
		else if (!strcmp(command, "HELP"))
		{
			printf("Available commands: PING, UNREGISTER, RESTART, SHUTDOWN\n");
		}
	}
}

DWORD WINAPI responseFunnel(LPVOID param){//create tcp connection and listen for responses
	//fprintf(stdout, "NEW THREAD\n");

	SOCKET nodeStream = **(SOCKET**)param;
	//printf("SOCKET: %d\n",nodeStream);
	//int node = rNodes.at(GetCurrentThread())->getNodeId();
	//rNodes.at(GetCurrentThread());
	int node = rNodesID[GetCurrentThread()];
	pkt p;
	char buffer[sizeof(pkt)];
	//fprintf(stdout,"GETTING INPUT\n");
	//sendCommand(nodeStream);
	while (true){
			//getResponse();
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
	//send_packet.header.timestamp = 0;
	memset(&packet.header.timestamp, 0, sizeof(float));

	int tmp = htonl(nextID-1);
	memcpy(packet.payload.data, &tmp, sizeof(tmp));
	htonPacket(packet, buffer);
	//fprintf(stdout, "\t* Assigning new node ID %d. Sending response...\n", num_nodes);
	//int sent = sendto(cli_sock, buffer, sizeof(struct pkt), 0, (struct sockaddr*)&masteraddr, sizeof(masteraddr));
	printf("sending ACK....");
	int r =send(sock, buffer, sizeof(struct pkt), 0);
	printf("sent %d bytes and error if 0!=%d\n",r,WSAGetLastError());
}

DWORD WINAPI registerThread(LPVOID param){
	struct sockaddr_in mc,  
		node;
	int slen;
	SOCKET *temp;
	struct sockaddr nodeName;

	//tSock = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	broadCastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	slen = sizeof(mc);

	mc.sin_family = AF_INET;
	mc.sin_addr.s_addr = INADDR_ANY;
	mc.sin_port = htons(UDP_PORT);

	bind(broadCastSocket, (struct sockaddr*)&mc, slen);
	//bind(tSock, (const struct sockaddr*)&mc, sizeof(mc));
	//listen(tSock, 1);//1 : 1 , thread : node

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
			fprintf(stdout,"Caught garbage broadcast\n");
			continue;
		}
		printf("assigning ID: %d\n", nextID);
		printf("to %s : %d\n\n", inet_ntoa(node.sin_addr), ntohs(node.sin_port));

		//assign ID to node
		*(p.payload.data) = nextID++;
		
		//create new connection
		temp = (SOCKET*)malloc(sizeof(SOCKET));
		*temp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(*temp<=0)
			printf("socket ERROR %d\n", WSAGetLastError());
		else
			printf("socket %d\n", *temp);

		node.sin_port = htons(TCP_PORT);
		int res = connect(*temp, (const sockaddr*)&node, sizeof(node));
		printf("SOCKET after conn: %d\n",*temp);
		if (res < 0)
			printf("connect ERROR %d\n", WSAGetLastError());
		//else
			//printf("Successful connection on SOCKET !\n");
		
		sendAck(*temp);
		
		//create new thread
		DWORD id;
		HANDLE thread;
		thread = CreateThread(0, 0, responseFunnel, &temp, 0, &id);
		
		//add stuff to sets
		int addr=node.sin_addr.s_addr;
		//printf("\naddr(int): %u\nid: %d\nsock: %d\n\n",a,nextID-1,*temp);
		addresses.insert(node.sin_addr.s_addr);
		stoaddr[*temp]=addr;
		idtosock[nextID-1]=*temp;
		socks.insert(*temp);
		addresses.insert(node.sin_addr.s_addr);
		printf("white listing %s\n", inet_ntoa(node.sin_addr));
		threads.insert(thread);
		//rNodes.insert(std::pair<HANDLE, RenderNode*>(thread, new RenderNode(nextID - 1)));
		rNodesID[thread] = nextID-1;
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
	for (std::set<SOCKET>::iterator i = socks.begin(); i != socks.end(); i++){
		closesocket(*i);
		//free(*i);
		++n;
	}
	printf("closed %d sockets\n", n);
	WSACleanup();
}

/*
int main(int argc, char** argv){
	enableRegistration();
	sendCommand();
	disableRegistration();
}*/