/* This program works as Http server.
 * It is capable of handling GET, PUT requests
 * 
 * @author  Soruabh Jain
 *	    M.Tech
 *	    Computer Science and Engineering	
 *          National Institute of Technology, Karnataka
 */




#include <stdio.h>
#include <stdlib.h>

// To avail Basic networking functionality
#include <sys/socket.h>
#include<arpa/inet.h>
#include <string.h>

// To get the cause of an error
#include<errno.h>

// To close the socket descriptor
#include <unistd.h>


// To get the file size using stat function
#include<sys/stat.h>

// To write the content of one file descriptor to another, using sendfile()
#include<sys/sendfile.h>

// To specify file access like:- O_RDONLY (open in read only mode)
#include<fcntl.h>
#include<netdb.h>
#include<signal.h>

#define CONNMAX 1000
#define BYTES 1024


// To track the path
char *ROOT;	

// Server and client socket descriptor		
int listenfd, clients[CONNMAX];
void startServer(char *);

// Handel the clients
void respond(int);

int main(int argc, char* argv[])
{
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	char c;
	int slot;
	int i;
	char PORT[6];
	ROOT = getenv("PWD");

	// Srever listen at port 10000
	strcpy(PORT,"10000");

	// Setting all elements to -1: signifies there is no client connected
	for (i=0; i<CONNMAX; i++)
	{
		clients[i]=-1;
	}

	// Put the server on listening mode
	startServer(PORT);

	while (1)
	{
	addrlen = sizeof(clientaddr);

	// Accept connection
	clients[slot] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

	if (clients[slot]<0)
	{	
		error ("accept() error");
	}
	else
	{
		// Create a child process to responed to client
		if ( fork()==0 )
		{
			// Handle the client at slot
			respond(slot);
			exit(0);
		}
        }

	// Set the slot element to -1 : signifies there is no client at this slot
        while (clients[slot]!=-1) slot = (slot+1)%CONNMAX;
    }

    return 0;
}

//start server
void startServer(char *port)
{
	struct addrinfo hints, *res, *p;

	// getaddrinfo for host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo( NULL, port, &hints, &res) != 0)
	{
		perror ("getaddrinfo() error");
		exit(1);
	}

	// socket and bind
	for(p = res; p!=NULL; p=p->ai_next)
	{
		listenfd = socket (p->ai_family, p->ai_socktype, 0);
		if(listenfd == -1)
		{
			continue;
		}

		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
		{
			break;
		}
    	}

	if (p==NULL)
	{
		perror ("socket() or bind()");
		exit(1);
	}
	
	freeaddrinfo(res);
	// listen for incoming connections
	if ( listen (listenfd, 1000000) != 0 )
	{
		perror("listen() error");
		exit(1);
	}
}


//client connection
void respond(int n)
{
	char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999];
	int rcvd, fd, bytes_read;

	// mesg buffer filled with '\0'
	memset( (void*)mesg, (int)'\0', 99999 );

	// Copy the content of socket descriptor clients[n] to mesg buffer
	rcvd=recv(clients[n], mesg, 99999, 0);

	// Error checking
	if (rcvd<0)  
	{
		fprintf(stderr,("recv() error\n"));
	}
	else if (rcvd==0)
	{
		fprintf(stderr,"Client disconnected upexpectedly.\n");
	}
	else
	{
		printf("%s", mesg);
		reqline[0] = strtok (mesg, " \t\n");
		if ( strncmp(reqline[0], "GET\0", 4)==0 )
		{
			reqline[1] = strtok (NULL, " \t");
			reqline[2] = strtok (NULL, " \t\n");
			if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
			{
				write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
			}
			else
			{
				if ( strncmp(reqline[1], "/\0", 2)==0 )
				{
				//Because if no file is specified, index.html will be opened by default (like it happens in APACHE...
				reqline[1] = "/index.html";        
				}

				strcpy(path, ROOT);
				strcpy(&path[strlen(ROOT)], reqline[1]);
				printf("file: %s\n", path);
				
				// File found
				if ( (fd=open(path, O_RDONLY))!=-1 )
				{
					send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
					while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
					write (clients[n], data_to_send, bytes_read);
				}
				else
				{
					//File not found
					write(clients[n], "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
				}
            		}
       		 }
    	}

	//Closing SOCKET : Disable future requests, it makes this server stateless
	shutdown (clients[n], SHUT_RDWR);
	close(clients[n]);
	clients[n]=-1;
}
