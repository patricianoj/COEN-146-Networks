/**************************
	Robbie Culkin
	Th 5:15 section

	File Transfer -version 1, Server Side.
	A client and a server that communicate to 
	transfer one file from 
	the client to the server using TCP.

	winter 2018
 **************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main (int, char *[]); 


/*********************
 * main
 *********************/
int main (int argc, char *argv[])
{
	int	n;
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr; 
	char name[10];
	char buff[5];

	if (argc != 2)
	{
		printf ("Usage: %s <port #> \n",argv[0]);
		return 1;
	}

	// set up
	memset (&serv_addr, '0', sizeof (serv_addr));
	memset (buff, '0', sizeof (buff)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	serv_addr.sin_port = htons (atoi(argv[1])); 

	// create socket, bind, and listen
	listenfd = socket (AF_INET, SOCK_STREAM, 0);
	bind (listenfd, (struct sockaddr*)&serv_addr, sizeof (serv_addr)); 
	listen (listenfd, 10); 

	// accept and interact
	connfd = accept (listenfd, (struct sockaddr*)NULL, NULL); 

	// receive filename & open the file
	read (connfd, name, sizeof (name));
	//if (n <= 0) printf("Error: no filename recieved");
	
	FILE *output;
	output = fopen(name, "wb");

	//recieve file data
	while ((n = read (connfd, buff, sizeof (buff))) > 0)
	{	
		fwrite(buff, sizeof(char), n, output);
	}

	close (connfd);
	fclose(output);
	
}
