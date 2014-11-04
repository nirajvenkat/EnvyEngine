#include "ENVY_TYPES.h"
#include "ENVY_CONSTANTS.h"
#include "ENVY_PACKET.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

void htonPacket(struct mgmt_pkt packet, char buffer[sizeof(struct mgmt_pkt)]);
void *TCPHandler(void* arg);

int didACK = 0;
pthread_mutex_t lock;

int main(int argc, char** argv)
{
	//Create TCP thread and mutex
	pthread_mutex_init(&lock, NULL);
	pthread_t server_thread;
	pthread_create(&server_thread, NULL, &TCPHandler, NULL);
	

	/*Set up UDP socket for broadcast until we can establish a reliable
	TCP connection with the master controller*/
	int udp_sock, udp_socklen, udp_status;
	struct sockaddr_in bcast;
	int yes;

	udp_socklen = sizeof(struct sockaddr_in);
	memset(&bcast, 0, udp_socklen);
	
	udp_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bcast.sin_addr.s_addr = htonl(INADDR_ANY);
	bcast.sin_port = htons(0);
	bcast.sin_family = PF_INET;

	udp_status = bind(udp_sock, (struct sockaddr *) &bcast, udp_socklen);
	udp_status = setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));

	bcast.sin_addr.s_addr=htonl(-1);
	bcast.sin_port = htons(UDP_PORT);
	bcast.sin_family = PF_INET;

	struct mgmt_pkt packet;
	packet.hdr.pkt_type = PKT_TYPE_STATUS;
	packet.hdr.node_id = 0;
	packet.hdr.status = STATUS_BOOT_OK;
	packet.hdr.timestamp = 0;
	char buffer[sizeof(struct mgmt_pkt)];
	htonPacket(packet, buffer);
	
	pthread_mutex_lock(&lock);
	while(!didACK)
	{
		udp_status = sendto(udp_sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &bcast, udp_socklen);
		pthread_mutex_unlock(&lock);
		sleep(BCAST_SLEEP);
		pthread_mutex_lock(&lock);
	}

	close(udp_sock);	
}

//Set up TCP socket to listen for incoming TCP connection from master controller
void *TCPHandler(void *args)
{
	int fd;
	struct sockaddr_in serv_addr, cli_addr;
	int yes = 1;

	if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(TCP_PORT);

	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		perror("setsockopt");
		exit(1);
	}

	if(bind(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	listen(fd, 1);
	int client = accept(fd, (struct sockaddr*) &cli_addr, sizeof(cli_addr));

	if(client < 0)
	{
		perror("accept");
		exit(1);
	}
	else
	{
		pthread_mutex_lock(&lock);
		didACK = 1;
		pthread_mutex_unlock(&lock);
		printf("ACCEPTED CONNECTION FROM CLIENT\n");
	}

}

//Convert fields in mgmt_pkt struct to network order and fill buffer
void htonPacket(struct mgmt_pkt packet, char buffer[sizeof(struct mgmt_pkt)])
{
	int offset = 0;

	uint16_t pkt_type = htons(packet.hdr.pkt_type);
	memcpy(buffer+offset, &pkt_type, sizeof(pkt_type));
	offset+= sizeof(pkt_type);

	uint16_t node_id = htons(packet.hdr.node_id);
	memcpy(buffer+offset, &node_id, sizeof(node_id));
	offset+= sizeof(node_id);

	uint16_t status = htons(packet.hdr.status);
	memcpy(buffer+offset, &status, sizeof(status));
	offset+= sizeof(status);

	uint32_t timestamp = htonl(packet.hdr.timestamp);
	memcpy(buffer+offset, &timestamp, sizeof(timestamp));
	offset+= sizeof(timestamp);

	memcpy(buffer+offset, &packet.payload.data, sizeof(packet.payload.data));
}
