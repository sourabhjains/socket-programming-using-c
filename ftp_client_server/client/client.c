/* This program works as client to file server
 * It is capable of sending, receiving files
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
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

// To get the file size using stat function
#include<sys/stat.h>

// To write the content of one file descriptor to another, using sendfile()
#include<sys/sendfile.h>

// To specify file access like:- O_RDONLY (open in read only mode)
#include<fcntl.h>

#define ERROR -1

int main(int argc,char *argv[])
{
	struct sockaddr_in server;
	struct stat obj;
	int serverFileDescriptor;
	int choice;
	char buf[100], command[5], filename[20], *f;
	int k, size, status;
	int filehandle;

	if((serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		perror("socket :");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = atoi(argv[1]);
	server.sin_addr.s_addr = 0;

	if(connect(serverFileDescriptor, (struct sockaddr*)&server, sizeof(server)) == ERROR)
	{
		perror("Connect :");
		exit(1);
	}
	int i = 1;

	while(1)
	{
		printf("Enter a choice:\n1- get\n2- put\n3- pwd\n4- ls\n5- cd\n6- quit\n");
		scanf("%d", &choice);
		switch(choice)
		{
			case 1:
				printf("Enter filename to get: ");
				scanf("%s", filename);
				strcpy(buf, "get ");
				strcat(buf, filename);
				send(serverFileDescriptor, buf, 100, 0);
				recv(serverFileDescriptor, &size, sizeof(int), 0);

				if(!size)
				{
					printf("No such file on the remote directory\n\n");
					break;
				}

				f = malloc(size);
				recv(serverFileDescriptor, f, size, 0);

				while(1)
				{
					filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
					if(filehandle == -1)
					{
						sprintf(filename + strlen(filename), "%d", i);
					}
					else
					{
						break;
					}
				}

				write(filehandle, f, size, 0);
				close(filehandle);
				strcpy(buf, "cat ");
				strcat(buf, filename);
				system(buf);
				break;

			case 2:
				printf("Enter filename to put to server: ");
				scanf("%s", filename);
				filehandle = open(filename, O_RDONLY);
				if(filehandle == -1)
				{
					printf("No such file on the local directory\n\n");
					break;
				}
				
				strcpy(buf, "put ");
				strcat(buf, filename);
				send(serverFileDescriptor, buf, 100, 0);
				stat(filename, &obj);
				size = obj.st_size;
				send(serverFileDescriptor, &size, sizeof(int), 0);
				sendfile(serverFileDescriptor, filehandle, NULL, size);
				recv(serverFileDescriptor, &status, sizeof(int), 0);
				if(status)
				{
					printf("File stored successfully\n");
				}
				else
				{
					printf("File failed to be stored to remote machine\n");
				}
				break;
			case 3:
				strcpy(buf, "pwd");
				send(serverFileDescriptor, buf, 100, 0);
				recv(serverFileDescriptor, buf, 100, 0);
				printf("The path of the remote directory is: %s\n", buf);
				break;

			case 4:
				strcpy(buf, "ls");
				send(serverFileDescriptor, buf, 100, 0);
				recv(serverFileDescriptor, &size, sizeof(int), 0);
				f = malloc(size);
				recv(serverFileDescriptor, f, size, 0);
				filehandle = open("temp.txt", O_CREAT|O_WRONLY, 0666);
				write(filehandle, f, size, 0);
				printf("The remote directory listing is as follows:\n");
				system("cat temp.txt");
				close(filehandle);
				break;

			case 5:
				strcpy(buf, "cd ");
				printf("Enter the path to change the remote directory: ");
				scanf("%s", buf + 3);
				send(serverFileDescriptor, buf, 100, 0);
				recv(serverFileDescriptor, &status, sizeof(int), 0);
				if(status)
				{
					printf("Remote directory successfully changed\n");
				}
				else
				{
					printf("Remote directory failed to change\n");
				}
				break;

			case 6:
				strcpy(buf, "quit");
				send(serverFileDescriptor, buf, 100, 0);
				recv(serverFileDescriptor, &status, 100, 0);
				if(status)
				{
					printf("Server closed\nQuitting..\n");
					exit(0);
				}
			printf("Server failed to close connection\n");
			close(serverFileDescriptor);
			
		}
	}
}

