#include "envy_network.h"

//Convert fields in pkt struct to network order and fill buffer
void htonPacket(struct pkt packet, char buffer[sizeof(struct pkt)])
{
	int offset = 0;

	uint16_t pkt_type = htons(packet.header.pkt_type);
	memcpy(buffer + offset, &pkt_type, sizeof(pkt_type));
	offset += sizeof(pkt_type);

	uint16_t status = htons(packet.header.status);
	memcpy(buffer + offset, &status, sizeof(status));
	offset += sizeof(status);

	uint32_t p_length = htonl(packet.header.p_length);
	memcpy(buffer + offset, &p_length, sizeof(p_length));
	offset += sizeof(p_length);

	memcpy(buffer + offset, packet.header.timestamp, sizeof(packet.header.timestamp));
	offset += sizeof(packet.header.timestamp);

	if (packet.header.p_length > 0)
	{
		memcpy(buffer + offset, packet.payload.data, MIN(packet.header.p_length, p_length));
	}
}

//Convert buffer containing pkt data to struct
struct pkt ntohPacket(char buffer[sizeof(struct pkt)])
{
	struct pkt packet;
	int offset = 0;


	uint16_t pkt_type;
	memcpy(&pkt_type, buffer + offset, sizeof(pkt_type));
	packet.header.pkt_type = ntohs(pkt_type);
	offset += sizeof(pkt_type);

	uint16_t status;
	memcpy(&status, buffer + offset, sizeof(status));
	packet.header.status = ntohs(status);
	offset += sizeof(status);

	uint32_t p_length;
	memcpy(&p_length, buffer + offset, sizeof(p_length));
	packet.header.p_length = ntohl(p_length);
	offset += sizeof(p_length);

	memcpy(packet.header.timestamp, buffer + offset, sizeof(packet.header.timestamp));
	offset += sizeof(packet.header.timestamp);

	if (packet.header.p_length > 0)
	{
		//causes crashes sometimes
		memcpy(packet.payload.data, buffer + offset, MIN(packet.header.p_length, p_length));
	}
	//fprintf(stdout, "Done converting\n");

	return packet;
}

//Convert float in host order to network ordered byte array
void htonFloat(char *bytes, float f)
{
	memcpy(bytes, &f, sizeof(f));

	if (getSystemEndianness() == ORDER_LITTLE_ENDIAN)
	{
		int i = 0;
		int j = sizeof(bytes)-1;
		while (i < j)
		{
			char tmp = bytes[i];
			bytes[i] = bytes[j];
			bytes[j] = tmp;
			i++;
			j--;
		}
	}
}

//Convert network ordered byte array to float
float ntohFloat(char *bytes)
{
	if (getSystemEndianness() == ORDER_LITTLE_ENDIAN)
	{
		int i = 0;
		int j = sizeof(bytes)-1;
		while (i < j)
		{
			char tmp = bytes[i];
			bytes[i] = bytes[j];
			bytes[j] = tmp;
			i++;
			j--;
		}
	}

	float *f = (float*)bytes;
	return *f;
}

//Get the endianness of this system. Returns 1 if little endian and 0 if big endian
int getSystemEndianness()
{
	short int test = 0x01;
	if (*(char*)&test == 1)
	{
		return ORDER_LITTLE_ENDIAN;
	}

	return ORDER_BIG_ENDIAN;
}