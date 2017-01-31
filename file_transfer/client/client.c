#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>


#define SOCKET_PORT "50000"
#define SOCKET_ADR "localhost"
#define filename "/home/sourabh/Desktop/input.txt"


void error(const char *msg)
{
    perror(msg);
    exit(0);
}


int main()
{
/* Making the client */
int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;

char buffer[256];

portno = atoi(SOCKET_PORT);

sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) 
    error("ERROR opening socket");

server = gethostbyname(SOCKET_ADR);

if (server == NULL) 
{
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
}

bzero((char *) &serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
bcopy((char *)server->h_addr, 
     (char *)&serv_addr.sin_addr.s_addr,
     server->h_length);
serv_addr.sin_port = htons(portno);
if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    error("ERROR connecting");

/* Time to send the file */
FILE *pf;
unsigned long fsize;

pf = fopen(filename, "rb");
if (pf == NULL) 
{
    printf("File not found!\n");
    return 1;
}
else 
{
    printf("Found file %s\n", filename);

    fseek(pf, 0, SEEK_END);
    fsize = ftell(pf);
    rewind(pf);

    printf("File contains %ld bytes!\n", fsize);
    printf("Sending the file now");
}

while (1) 
{
    // Read data into buffer.  We may not have enough to fill up buffer, so we
    // store how many bytes were actually read in bytes_read.
    int bytes_read = fread(buffer, sizeof(char),sizeof(buffer), pf);
    if (bytes_read == 0) // We're done reading from the file
        break;

    if (bytes_read < 0) 
    {
        error("ERROR reading from file"); 
    }

    // You need a loop for the write, because not all of the data may be written
    // in one call; write will return how many bytes were written. p keeps
    // track of where in the buffer we are, while we decrement bytes_read
    // to keep track of how many bytes are left to write.
    void *p = buffer;
    while (bytes_read > 0) 
    {
        int bytes_written = write(sockfd, buffer, bytes_read);
        if (bytes_written <= 0) 
        {
            error("ERROR writing to socket\n");
        }
        bytes_read -= bytes_written;
        p += bytes_written;
    }
}       

printf("Done Sending the File!\n");
printf("Now Closing Connection.\n");

fclose(pf);
close(sockfd);
return 0;
}
