#include <stdint.h>

struct mgmt_hdr
{
	uint16_t pkt_type;
	uint16_t node_id;
	uint16_t status;
	uint32_t timestamp;
};

struct mgmt_payload
{
	char data[128];
};

struct mgmt_pkt
{
	struct mgmt_hdr hdr;
	struct mgmt_payload payload;
};

