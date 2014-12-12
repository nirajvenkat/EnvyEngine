#include <stdint.h>
#include "ENVY_CONSTANTS.h"
#include "math3d.h"

struct pkt_hdr //Packet header
{
	uint16_t pkt_type; //Type of packet
	uint16_t status; //Status or error code
	uint32_t p_length; //Length of payload
	unsigned char timestamp[TIMESTAMP_SIZE]; //Char array holding network-ordered timestamp float
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

struct pkt_command_payload {
	unsigned long taskSeq;
	double taskTime;
	Matrix4f taskMatrix;
};
