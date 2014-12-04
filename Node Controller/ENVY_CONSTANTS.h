#define PAYLOAD_SIZE	128	//Size in bytes of payload in packet struct
#define TIMESTAMP_SIZE	4	//Size in bytes of timestamp char array in packet struct. Equal to sizeof(float)

#define	UDP_PORT	9998	//Port on which nodes broadcast UDP
#define TCP_PORT	9999	//Port on which nodes listen for TCP connection
#define BCAST_SLEEP	2	//Seconds to sleep after UDP broadcast
#define RECV_TIMEOUT	3	//Seconds to wait for a response from node
