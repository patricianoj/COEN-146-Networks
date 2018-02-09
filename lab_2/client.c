/**************************
	Robbie Culkin
	Th 5:15 section

	File Transfer -version 1, Client Side.
	A client and a server that communicate to 
	transfer one file from 
	the client to the server using TCP.

	winter 2018
 **************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main (int, char *[]);


/********************
 * main
 ********************/
int main (int argc, char *argv[])
{
	int i;
	int sockfd = 0, n = 0;
	char buff[10];
	char *p;
	struct sockaddr_in serv_addr; 

	if (argc != 5)
	{
		printf ("Usage: %s <input> <output> <ip of server> <port #> \n",argv[0]);
		return 1;
	}

	// set up
	memset (buff, '0', sizeof (buff));
	memset (&serv_addr, '0', sizeof (serv_addr)); 

	// open socket
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("Error : Could not create socket \n");
		return 1;
	} 

	// set address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons (atoi(argv[4]));

	if (inet_pton (AF_INET, argv[3], &serv_addr.sin_addr) <= 0)
	{
		printf ("inet_pton error occured\n");
		return 1;
	} 

	// connect
	if (connect (sockfd, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0)
	{
		printf ("Error : Connect Failed \n");
		return 1;
	} 

	// read file
	FILE *input;
	input = fopen(argv[1], "rb");
	
	//give server the filename
	write (sockfd, argv[2], strlen (argv[2]) + 1);
	
	//send to server
	while (!feof(input))
	{
		n = fread(buff, sizeof(char), 10, input);
		write (sockfd, buff, n);
	}
	fclose(input);

	return 0;
}
