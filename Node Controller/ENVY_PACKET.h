#include <stdint.h>
#include "ENVY_CONSTANTS.h"

struct pkt_hdr //Packet header
{
	uint16_t pkt_type; //Type of packet
	//uint16_t node_id; //ID of sending or receiving node
	uint16_t status; //Status or error code
	uint32_t p_length; //Length of payload
	uint32_t timestamp; //Timestamp
};

struct pkt_payload //Packet payload
{
	unsigned char data[PAYLOAD_SIZE]; //Data buffer
};

struct pkt //Packet
{
	struct pkt_hdr header; 
	struct pkt_payload payload;
};

