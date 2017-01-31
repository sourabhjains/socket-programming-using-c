/* This program works as a echo server.
 *
 * @author  Soruabh Jain
 *	    M.Tech
 *	    Computer Science and Engineering	
 *          National Institute of Technology, Karnataka
 */




#include<stdio.h>
#include<stdlib.h>

// To use basic networking functions like:- socket, listen, bind, accept, send, recv
#include<sys/socket.h>

// To get the cause of an error
#include<errno.h>

// To perform conversion of data like:- network to host, ascii to host
#include<arpa/inet.h>

// To handle the file descriptors
#include<unistd.h>

#define ERROR -1
#define DATA_LENGTH 512

int main(int argc, char** argv)
{
	struct sockaddr_in server;     // Store server socket information
	struct sockaddr_in client;     // Store client socket information
	int sock;		       // Server socket descriptor
	int new;		       // Client socket descriptor
	socklen_t sockaddr_len = sizeof(struct sockaddr_in);
	int data_len;
	char data[DATA_LENGTH];           // Buffer to hold data

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)     // Creating socket by giving address type, transmission mechanism, protocol 
	{
		perror("Server socket : ");			  
		exit(-1);
	}
							// Assigning the values to the server socket
	server.sin_family = AF_INET;			// Address type (AF_INET -> ipv4)
	server.sin_port = htons(atoi(argv[1]));		// Port number (atoi - ascii to integer , htons - integer to network byte order)
	server.sin_addr.s_addr  = INADDR_ANY;		// Interface 
	bzero(&server.sin_zero, 8);
	
	if((bind(sock, (struct sockaddr *)&server, sockaddr_len)) == ERROR)     // Binding the server socket to a scoket descriptor
	{
		perror("Bind : ");
		exit(-1);
	}

	if((listen(sock, 1)) == ERROR)						// Putting socket on listening mode
	{		
		perror("Listen :");
		exit(-1);
	}

	while(1)
	{
		new = accept(sock, (struct sockaddr *)&client, &sockaddr_len);	// Accepts the first pending connection in the queue
		if(new == ERROR)						// Accept method also set the address and port number-
		{								// to client socket descriptor
			perror("accept :");
			exit(-1);		
		}
		
		printf("New client connected from port no %d and IP %s \n", ntohs(client.sin_port), inet_ntoa(client.sin_addr));
		data_len = 1;
		while(data_len)
		{
			data_len = recv(new, data, DATA_LENGTH, 0);		// Reading the data from client socket descriptor
			if(data_len)	
			{							// new is client socket descriptor
				send(new, data, data_len, 0);			// Send the same data back to client
				data[data_len] = '\0';
				printf("Sent message : %s", data);		
			}
		}
		printf("Client Disconnected.\n");
		close(new);							// Closing  client file descriptor
	}
	close(sock);								// Closing server file descriptor
	
}

