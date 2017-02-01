/* This program works as file server.
 * It is capable of sending, receiving files and executing basic commands.
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

#define ERROR -1

int main(int argc,char *argv[])
{
	struct sockaddr_in server;	// Store server socket information
	struct sockaddr_in client;	// Store client socket information

	int serverFileDescriptor, clientFileDescriptor;	// Client and Server file descriptor
	char buf[100], command[5], filename[20];
	int k, i, size, sockaddr_len, c;
	int filehandle;
	struct stat fileInfo;

	if((serverFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)	//Create socket
	{
		perror("Socket :");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = atoi(argv[1]);
	server.sin_addr.s_addr = INADDR_ANY;
      	bzero(&server.sin_zero, 8);

	if(bind(serverFileDescriptor, (struct sockaddr*)&server, sizeof(server)) == ERROR)	// Bind server socket des. with port and address
	{
		perror("Bind :");
		exit(1);
	}

	if(listen(serverFileDescriptor, 1) == ERROR)	// Put the server on listening mode
	{
		perror("Listen :");
		exit(1);
	}

	sockaddr_len = sizeof(client);			// Accepts the first pending connection in the queue
	clientFileDescriptor = accept(serverFileDescriptor, (struct sockaddr*)&client, &sockaddr_len); 

	while(1)
	{
		recv(clientFileDescriptor, buf, 100, 0);	// Copy the content of CFD to buf (in this case it a command)
		sscanf(buf, "%s", command);			// Copy the content of buffer to command array
		if(!strcmp(command, "ls"))			// strcmp returns zero if string match else 1 or -1
		{
			system("ls >temps.txt");		// System call to write the list of available files in current working -
								// to temps.txt file
			stat("temps.txt", &fileInfo);		// stat gives information about file
			size = fileInfo.st_size;		// Extracting file size
			send(clientFileDescriptor, &size, sizeof(int), 0);	// Sending file size to client side
			filehandle = open("temps.txt", O_RDONLY);		// Open assign a file descriptor to a file and return -
										// that descriptor 								 
			sendfile(clientFileDescriptor, filehandle, NULL, size); // Write the content of one file descriptor to another
		}
		else if(!strcmp(command,"get"))
		{
			sscanf(buf, "%s%s", filename, filename);    // To handle the special character in the file name two times same -
								    // variable name is given	
			stat(filename, &fileInfo);		    	
			filehandle = open(filename, O_RDONLY);
			size = fileInfo.st_size;
			if(filehandle == -1)
			{
				size = 0;
			}
			send(clientFileDescriptor, &size, sizeof(int), 0);
			if(size)
			{		
	  			sendfile(clientFileDescriptor, filehandle, NULL, size); //Write the content of filehandle file descriptor to
											// clientFileDescriptor
			}
      
		}
		else if(!strcmp(command, "put"))	
		{
			int c = 0, len;
			char *f;
			sscanf(buf+strlen(command), "%s", filename);	// Copying the file name to filename array by eliminating put command
			recv(clientFileDescriptor, &size, sizeof(int), 0);   // Receive file size
			i = 1;
			while(1)					     // It checks the availabiltiy of file
			{
				filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);   // Open return -1 if file already exists 
				if(filehandle == -1)		
				{
					sprintf(filename + strlen(filename), "%d", i);	// If file exists then append 1 at last and check again
				}
				else
				{
					break;			// It breaks when open return a unique file descriptor		
				}
			}
			f = malloc(size);			// Create a dynamic buffer
			recv(clientFileDescriptor, f, size, 0); // Copy the content of clientFileDescriptor to buffer
			c = write(filehandle, f, size);		// Write the content of buffer to file
			close(filehandle);			// Close the file
			send(clientFileDescriptor, &c, sizeof(int), 0);	// If c is non-zero it indicates that file copied successfully 
		}
		else if(!strcmp(command, "pwd"))
		{
			system("pwd>temp.txt");			//Get the current working directory and store it in temp.txt file
			i = 0;
			FILE *f = fopen("temp.txt","r");	// Open temp.txt in read mode
			while(!feof(f))				// Read the full content of file 
			{					
				buf[i++] = fgetc(f);		// Write every character in buffer
			}
			buf[i-1] = '\0';
			fclose(f);				// Close the file
			send(clientFileDescriptor, buf, 100, 0); // Send the buffer data back to the client
		}
		else if(!strcmp(command, "cd"))
		{
			if(chdir(buf+3) == 0)			// Change the current working directory, on success zero is returned.
			{
				c = 1;
			}
			else
			{
				c = 0;
			}

			send(clientFileDescriptor, &c, sizeof(int), 0);	// Return 1 to client if directory changed else 0 
		}
		else if(!strcmp(command, "quit"))
		{
			printf("FTP server quitting..\n");
			i = 1;
			send(clientFileDescriptor, &i, sizeof(int), 0);
			close(serverFileDescriptor);
			close(clientFileDescriptor);
			exit(0);
		}
	}

	return 0;
}
