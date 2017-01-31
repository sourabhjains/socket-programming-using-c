/* This program works as a client for an echo server.
 *
 * @author  Soruabh Jain
 *	    M.Tech
 *	    Computer Science and Engineering	
 *          National Institute of Technology, Karnataka
 */




#include<stdio.h>
#include<stdlib.h>

// To use basic networking functions like:- socket, connect, send, recv
#include<sys/socket.h>

// To get the cause of an error
#include<errno.h>

// To perform operations on string like:- strlen
#include<string.h>

// To perform conversion of data like:- network to host, ascii to host
#include<arpa/inet.h>

// To handle the file descriptors
#include<unistd.h>

#define ERROR -1
#define BUFFER 512


int main(int argc, char **argv)
{
	struct sockaddr_in server;	// Store server socket information
	int sock;			// Server socket descriptor
	char input[BUFFER];		
	char output[BUFFER];
	int len;
	
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)	// Create socket by giving address type, transmission mechanism, protocol
	{
		perror("Socket :");		
		exit(-1);
	}
							// Assigning the values to the server socket
	server.sin_family = AF_INET;			// Address type (AF_INET -> ipv4)
	server.sin_port = htons(atoi(argv[2]));		// Port number (atoi - ascii to integer , htons - integer to network byte order)
	server.sin_addr.s_addr = inet_addr(argv[1]);	// Interface
	bzero(&server.sin_zero, 8);
	
	if((connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in))) == ERROR)  // Connects the socket descriptor and server addr
	{
		perror("Connect :");
		exit(-1);
	}

	while(1)
	{
		fgets(input, BUFFER, stdin);		// Take input stirng from user
		send(sock, input, strlen(input), 0);	// Write the content of input to sock socket descriptor and send it
		len = recv(sock, output, BUFFER, 0);	// Copy the content of file descriptor to output buffer
		output[len] = '\0';		
		printf("%s", output);
	}
	
	close(sock);					// Close the server descriptor
}

