/*
Sam's code ported

broadcasts are received by the driver
tcp connection is established

TODO
figure out why commands aren't being received
spinlocks could be more efficient

*/


#include "envy_network.h"
#include "envy_server.h"
#include <sys/types.h>
#include <time.h>
#include "renderer.h"
#include "renderTask.h"

extern Renderer *gRenderer;

int didACK = false;
int NODE_ID = 0;

//pthread_mutex_t lock;
volatile HANDLE lock;

int nodeMain()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	//Create TCP thread and mutex
	//pthread_mutex_init(&lock, NULL);
	lock = CreateMutex(NULL,FALSE,NULL);

	//pthread_t server_thread;
	HANDLE server_thread;
	DWORD server_thread_id;


	/*Set up UDP socket for broadcast until we can establish a reliable
	TCP connection with the master controller*/
	int udp_sock, udp_socklen, udp_status;
	struct sockaddr_in bcast;
	int yes = true;

	udp_socklen = sizeof(struct sockaddr_in);
	memset(&bcast, 0, udp_socklen);

	udp_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bcast.sin_addr.s_addr = htonl(INADDR_ANY);
	bcast.sin_port = htons(0);
	bcast.sin_family = PF_INET;

	udp_status = bind(udp_sock, (struct sockaddr *) &bcast, udp_socklen);
	udp_status = setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, (const char*)&yes, sizeof(yes));

	bcast.sin_addr.s_addr = htonl(-1);
	bcast.sin_port = htons(UDP_PORT);
	bcast.sin_family = PF_INET;

	struct pkt packet;
	packet.header.pkt_type = PKT_TYPE_STATUS;
	packet.header.status = STATUS_BOOT_OK;
	packet.header.p_length = 0;
	float timestamp = 0.00;
	char timestamp_bytes[sizeof(float)];
	htonFloat(timestamp_bytes, timestamp);

	//packet.header.timestamp = timestamp_bytes;
	memcpy(&packet.header.timestamp, &timestamp_bytes, sizeof(float));

	char buffer[sizeof(struct pkt)];
	htonPacket(packet, buffer);


	while (true)
	{
		fprintf(stdout, "Beginning broadcast on port %d...\n", UDP_PORT);
		
		//pthread_create(&server_thread, NULL, &TCPHandler, NULL);
		server_thread = CreateThread(0, 0, TCPHandler, NULL,0, &server_thread_id);
		
		//pthread_mutex_lock(&lock);
		// no timeout
		while (WaitForSingleObject(lock, INFINITE) != WAIT_OBJECT_0);
			//fprintf(stdout, "SPINNING\n");
		//printf("%d %s\n", GetLastError(), GetLastError());

		while (!didACK)
		{
			fprintf(stdout, "BROADCASTING\n");

			udp_status = sendto(udp_sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &bcast, udp_socklen);
			if(udp_status <=0)
				fprintf(stdout, "ERROR %d\n", WSAGetLastError());
			
			//pthread_mutex_unlock(&lock);
			ReleaseMutex(lock);
			
			Sleep(BCAST_SLEEP*5);

			//pthread_mutex_lock(&lock);
			while (WaitForSingleObject(lock, INFINITE) != WAIT_OBJECT_0);
		}

		//pthread_join(server_thread, NULL);
		WaitForSingleObject(server_thread,INFINITE);
		//while(didACK)
			//printf("WINNER\n");
		fprintf(stdout, "TCP thread exited. Begin rebroadcast...\n");
		while (WaitForSingleObject(lock, INFINITE) != WAIT_OBJECT_0);
		didACK = false;
		ReleaseMutex(lock);
	}

	closesocket(udp_sock);
	WSACleanup();
}

//Set up TCP socket to listen for incoming TCP connection from master controller
DWORD WINAPI TCPHandler(void *args)
{
	fprintf(stdout, "TCP listener thread running! Waiting for connection on port %d.\n", TCP_PORT);

	int fd;
	struct sockaddr_in serv_addr, cli_addr;
	int yes = 1;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(TCP_PORT);

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes)) < 0)
	{
		perror("setsockopt");
		exit(1);
	}

	if (bind(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	listen(fd, 1);
	int clilen = sizeof(cli_addr);
	int client = accept(fd, (struct sockaddr*) &cli_addr, &clilen);

	if (client < 0)
	{
		perror("accept");
		exit(1);
	}
	else
	{
		fprintf(stdout, "CONNECTION ACCEPTED\n");

		//pthread_mutex_lock(&lock);
		while (WaitForSingleObject(lock, INFINITE) != WAIT_OBJECT_0){fprintf(stdout,"spinning\n");}
			//fprintf(stdout, "SPINNING\n");
		//printf("%d %s\n", GetLastError(), GetLastError());
		//fprintf(stdout, "THREAD LOCK OBTAINED\n");

		didACK = true;
		//pthread_mutex_unlock(&lock);
		ReleaseMutex(lock);
		//printf("%d %s\n", GetLastError(), GetLastError());

		char buffer[sizeof(struct pkt)];
		//recvfrom(client, buffer, sizeof(buffer), 0, (struct sockaddr*)&cli_addr, &clilen);
		pkt recv_packet;

		//struct pkt recv_packet = ntohPacket(buffer);
		int r;
		r = recv(client, (char*)&recv_packet, sizeof(recv_packet), 0);

		int tmp;
		//memcpy(&tmp, recv_packet.payload.data, sizeof(tmp));
		//NODE_ID = ntohl(tmp);
		NODE_ID = (int)recv_packet.payload.data[0];
		fprintf(stdout, "Received acknowledgement from master controller! My node ID is %d!\n", NODE_ID);

		int finished = false;
		int status;

		struct pkt send_packet;

		while (!finished) //Loop until we receive a stop command from master
		{
			//recvfrom(client, buffer, sizeof(buffer), 0, (struct sockaddr*)&cli_addr, &clilen);
			//recv_packet = ntohPacket(buffer);
			recv(client, (char*)&send_packet, sizeof(send_packet), 0);
			status = -1;
			//memset(&send_packet, 0, sizeof(send_packet));

			//Handle each type of packet differently
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
				case CMD_RESTART:
					send_packet.header.pkt_type = PKT_TYPE_STATUS;
					send_packet.header.status = STATUS_SHUTTING_DOWN;
					send_packet.header.p_length = 0;

					//send_packet.header.timestamp = recv_packet.header.timestamp;
					memcpy(&send_packet.header.timestamp, &recv_packet.header.timestamp, sizeof(float));

					htonPacket(send_packet, buffer);
					status = sendto(client, buffer, sizeof(buffer), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
					fprintf(stdout, "Received RESTART from Master; Rebooting machine.\n");
					finished = true;
					closesocket(fd);
					//reboot(LINUX_REBOOT_CMD_RESTART);
					break;
				case CMD_SHUTDOWN:
					send_packet.header.pkt_type = PKT_TYPE_STATUS;
					send_packet.header.status = STATUS_SHUTTING_DOWN;
					send_packet.header.p_length = 0;
					//send_packet.header.timestamp = recv_packet.header.timestamp;
					htonPacket(send_packet, buffer);
					status = sendto(client, buffer, sizeof(buffer), 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
					fprintf(stdout, "Received SHUTDOWN from Master; Powering off machine.\n");
					finished = true;
					closesocket(fd);
					//reboot(LINUX_REBOOT_CMD_POWER_OFF);
					break;
				case CMD_UNAME:
					send_packet.header.pkt_type = PKT_TYPE_STATUS;
					send_packet.header.status = STATUS_OK;
					/*
					struct utsname utsnameData;
					uname(&utsnameData);

					memcpy(send_packet.payload.data, utsnameData.sysname, strlen(utsnameData.sysname));
					strcat(send_packet.payload.data, " ");
					send_packet.header.p_length = strlen(utsnameData.sysname)+3;

					strcat(send_packet.payload.data, utsnameData.release);
					strcat(send_packet.payload.data, " ");
					send_packet.header.p_length += strlen(utsnameData.release)+3;

					strcat(send_packet.payload.data, utsnameData.machine);
					strcat(send_packet.payload.data, " ");
					send_packet.header.p_length += strlen(utsnameData.machine)+3;

					send_packet.header.timestamp = recv_packet.header.timestamp;
					htonPacket(send_packet, buffer);
					status = sendto(client,buffer,sizeof(buffer),0,(struct sockaddr *)&cli_addr, sizeof(cli_addr));
					fprintf(stdout, "Received UNAME from Master; Sending back response.\n");
					*/
					break;
				}
			}
			else if (recv_packet.header.pkt_type == PKT_TYPE_STATUS)
			{
				//We got a status update or statistic...
			}

		}
	}
}
