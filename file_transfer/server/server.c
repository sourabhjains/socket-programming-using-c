#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <strings.h>


#define SOCKET_PORT 50000
#define filename "/home/sourabh/Desktop/output.txt"


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void* client_thread_proc(void* arg)
{
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int n;
FILE *fp;

int thisfd = (int)arg;
printf("Server %d: accepted = %d\n", getpid(), thisfd);

if (thisfd < 0) 
{ 
    printf("Accept error on server\n");
    error("ERROR on accept"); 
    return NULL;
}

printf("Connection %d accepted\n", thisfd);

fp = fopen(filename, "a+b");
if (fp == NULL) 
{
    printf("File not found!\n");
    return NULL;
}
else 
{
    printf("Found file %s\n", filename);
}

/* Time to Receive the File */
while (1)
{
    bzero(buffer,256);
    n = read(thisfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");

    n = fwrite(buffer, sizeof(char), sizeof(buffer), fp);
    if (n < 0) error("ERROR writing in file");

    n = write(thisfd,"I am getting your file...",25);
    if (n < 0) error("ERROR writing to socket");
} /* end child while loop */

fclose(fp);

return NULL;
}

void serve_it(int Client)
{
    void* arg = (void*)Client;
    pthread_t new_thread;
    pthread_create( &new_thread, NULL, &client_thread_proc, arg);
}

/* Making Server */
int main()
{
int sockfd, newsockfd, portno;
socklen_t clilen;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int n;
FILE *fp;

signal (SIGCHLD, SIG_IGN);

sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) 
    error("ERROR opening socket");

bzero((char *) &serv_addr, sizeof(serv_addr));

serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port = htons(SOCKET_PORT);

if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
      error("ERROR on binding");

listen(sockfd,5);

clilen = sizeof(cli_addr);

while (1)
{
    printf("Server %d accepting connections\n", getpid());

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    serve_it(newsockfd);
}  // serving loop


close(sockfd);   
return 0; 
}

