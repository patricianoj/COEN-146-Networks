// Robbie Culkin
// COEN 146 Th 5:15
// server (reciever) side of UDP file transfer with rdt 2.2

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

int main (int, char *[]); 

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

/*********************
 * main
 *********************/

int main (int argc, char *argv[])
{
	int sock, nBytes;
	PACKET buf;
	PACKET resp;
	PACKET* buffer = &buf;
	PACKET* response = &resp;
	struct sockaddr_in serverAddr, clientAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	int i;

	if (argc != 2)
	{
		printf ("Usage: %s <port #> \n",argv[0]);
		return 1;
	}

	// init 
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons ((short)atoi (argv[1]));
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	memset ((char *)serverAddr.sin_zero, '\0', sizeof (serverAddr.sin_zero));  
	addr_size = sizeof (serverStorage);

	// create socket
	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf ("socket error\n");
		return 1;
	}

	// bind
	if (bind (sock, (struct sockaddr *)&serverAddr, sizeof (serverAddr)) != 0)
	{
		printf ("bind error\n");
		return 1;
	}
	
	int len;
	int seq_ack;
	char data[10];
	int first = 1;
	FILE *output;

	do{
		nBytes = recvfrom (sock, buffer, sizeof(PACKET), 0, (struct sockaddr *)&clientAddr, &addr_size);	

		seq_ack	= (*buffer).header.seq_ack;
		len = (*buffer).header.length;
		strcpy(data, (*buffer).data);
		printf("data recieved: %s\n",data);


		//checksum
		int pkt_checksum = (*buffer).header.checksum;
		(*buffer).header.checksum = 0;
		
		printf("pkt chksum:        %d\n",pkt_checksum);
		printf("calculated chksum: %d\n",checksum(buffer,sizeof(PACKET)));		
		if (checksum(buffer,sizeof(PACKET)) != pkt_checksum)
		{
			(*response).header.seq_ack = (seq_ack+1)%2;
			printf("CHECKSUM FAILED!\n");
			sendto (sock, response, sizeof(PACKET), 0, (struct sockaddr *)&clientAddr, addr_size);

			continue;
		}

		//ACK
		if(rand()%100 < 80 && len!=0) //20% of the time, don't send ACK (simulate packet drop)
		{
				(*response).header.seq_ack = seq_ack;
				sendto (sock, response, sizeof(PACKET), 0, (struct sockaddr *)&clientAddr, addr_size);
		}
		else {
			printf("DROPPING PACKET\n");
			continue;
		}

		//make file if first transmission
		if (first)
		{
			first = 0;
			output = fopen(data, "wb");
		}
		//else add to the file
		else
		{
			fwrite(data, sizeof(char), len, output);
		}
		

	}while(len > 0);

	fclose(output);
	return 0;
	
}
