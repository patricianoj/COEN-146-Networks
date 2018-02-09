// Robbie Culkin
// COEN 146 Th 5:15
// client (sender) side of UDP file transfer with rdt 2.2

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main (int argc, char *argv[]);

typedef struct
{
	int seq_ack;
	int length;
	int checksum;
} HEADER;
typedef struct
{
	HEADER header;
	char data[10];
} PACKET;

int checksum(PACKET* pkt, size_t size)
{
	(*pkt).header.checksum = 0;
	char* head = (char*)pkt;
	char sum = head[0];
	
	int i;
	for(i=1; i<size; i++)
		sum ^= head[i];
	
	return (int)sum;
}

/***********
 *  main
 ***********/
int main (int argc, char *argv[])
{
	
	int sock, portNum, nBytes;
	char data[10];
	PACKET buf;
	PACKET resp;
	PACKET *buffer = &buf;
	PACKET *response = &resp;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;


	if (argc != 5)
	{
		printf ("Usage: %s <input> <output> <ip of server> <port #> \n",argv[0]);
		return 1;
	}

	// configure address
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (atoi (argv[4]));
	inet_pton (AF_INET, argv[3], &serverAddr.sin_addr.s_addr);
	memset (serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));  
	addr_size = sizeof serverAddr;

	/*Create UDP socket*/
	sock = socket (PF_INET, SOCK_DGRAM, 0);

	// open file
	FILE *input;
	input = fopen(argv[1], "rb");
	
	int first = 1;
	int state = 0;
	int server_ack;
	int len;
	

	do
	{
		// send filename on first transmission
		if (first)
		{
			strcpy((*buffer).data, argv[2]);
		}
		else // otherwise send file, 10 bytes at a time
		{
			// read data
			len = fread(data, sizeof(char), 10, input);
			// build packet
			strcpy((*buffer).data, data);
		}
		
		(*buffer).header.seq_ack = state;
		(*buffer).header.length = len;
		
		do 
		{
			(*buffer).header.checksum = 0;
			(*buffer).header.checksum = checksum(buffer,sizeof(PACKET));
			printf("calculated chksum:%d\n",(*buffer).header.checksum);

			if(rand()%100 < 20) //20% of the time, send a bogus checksum
			{
				(*buffer).header.checksum = 0;
				printf("SENDING BOGUS CHECKSUM\n");
			}

			// send
			sendto (sock, buffer, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
			printf("pkt sent!\n");

			// receive ack
			recvfrom (sock, response, sizeof(PACKET), 0, NULL, NULL);
			
		} while((*response).header.seq_ack != state); //didnt get correct ack back? do it again
		first = 0;
		printf("valid ack received\n");
		state = (state+1)%2;

	} while(!feof(input)); 


	//send empty message to indicate file close
	(*buffer).header.seq_ack = state;
	(*buffer).header.length = 0;
	(*buffer).header.checksum = 0;
	(*buffer).header.checksum = checksum(buffer,sizeof(PACKET));
	sendto (sock, buffer, sizeof(PACKET), 0, (struct sockaddr *)&serverAddr, addr_size);
			
	
	return 0;
}
