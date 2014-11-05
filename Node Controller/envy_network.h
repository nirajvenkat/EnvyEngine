#include "ENVY_TYPES.h"
#include "ENVY_CONSTANTS.h"
#include "ENVY_PACKET.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <inttypes.h>

void htonPacket(struct pkt packet, char buffer[sizeof(struct pkt)]);
struct pkt ntohPacket(char buffer[sizeof(struct pkt)]);


//Convert fields in pkt struct to network order and fill buffer
void htonPacket(struct pkt packet, char buffer[sizeof(struct pkt)])
{
	int offset = 0;

	uint16_t pkt_type = htons(packet.header.pkt_type);
	memcpy(buffer+offset, &pkt_type, sizeof(pkt_type));
	offset+= sizeof(pkt_type);

	/*uint16_t node_id = htons(packet.header.node_id);
	memcpy(buffer+offset, &node_id, sizeof(node_id));
	offset+= sizeof(node_id);*/

	uint16_t status = htons(packet.header.status);
	memcpy(buffer+offset, &status, sizeof(status));
	offset+= sizeof(status);

	uint32_t p_length = htons(packet.header.p_length);
	memcpy(buffer+offset, &p_length, sizeof(p_length));
	offset+= sizeof(p_length);

	uint32_t timestamp = htonl(packet.header.timestamp);
	memcpy(buffer+offset, &timestamp, sizeof(timestamp));
	offset+= sizeof(timestamp);

	memcpy(buffer+offset, packet.payload.data, packet.header.p_length);
}

//Convert buffer containing pkt data to struct
struct pkt ntohPacket(char buffer[sizeof(struct pkt)])
{
	struct pkt packet;
	int offset = 0;

	uint16_t pkt_type;
	memcpy(&pkt_type, buffer+offset, sizeof(pkt_type));
	packet.header.pkt_type = ntohs(pkt_type);
	offset += sizeof(pkt_type);

	/*uint16_t node_id;
	memcpy(&node_id, buffer+offset, sizeof(node_id));
	packet.header.node_id = ntohs(node_id);
	offset += sizeof(node_id);*/

	uint16_t status;
	memcpy(&status, buffer+offset, sizeof(status));
	packet.header.status = ntohs(status);
	offset += sizeof(status);

	uint32_t p_length;
	memcpy(&p_length, buffer+offset, sizeof(p_length));
	packet.header.p_length = ntohs(p_length);
	offset += sizeof(p_length);

	uint32_t timestamp;
	memcpy(&timestamp, buffer+offset, sizeof(timestamp));
	packet.header.timestamp = ntohl(timestamp);
	offset += sizeof(timestamp);

	memcpy(packet.payload.data, buffer+offset, packet.header.p_length);
	return packet;
}
