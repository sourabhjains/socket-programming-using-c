#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<errno.h>
#include <arpa/inet.h>


#define ERROR -1
#define BUFFER 1024


int main(int argc, char **argv)
{
	struct sockaddr_in remoteServer;
	int sock;
	char input[BUFFER];
	char output[BUFFER];
	int len;
	
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		perror("Socket :");		
		exit(-1);
	}
	
	remoteServer.sin_family = AF_INET;
	remoteServer.sin_port = htons(atoi(argv[2]));
	remoteServer.sin_addr.s_addr = inet_addr(argv[1]);
	bzero(&remoteServer.sin_zero, 8);
	
	if((connect(sock, (struct sockaddr *)&remoteServer, sizeof(struct sockaddr_in))) == ERROR)
	{
		perror("Connect :");
		exit(-1);
	}

	while(1)
	{
		fgets(input, BUFFER, stdin);
		send(sock, input, strlen(input), 0);
		len = recv(sock, output, BUFFER, 0);
		output[len] = '\0';		
		printf("%s", output);
	}
	
	close(sock);
}

