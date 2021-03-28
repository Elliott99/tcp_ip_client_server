#include <netdb.h>
#include "server.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"

#define MAX_MESSAGE_LENGTH 200
void chat_function(int sockfd,char *write_buffer,char *read_buffer){
	printf("\nEnter a message to send to the server. The server will reverse it, and send it back\n");
	fscanf(stdin,"%s",write_buffer);
	ssize_t m=strlen(write_buffer);
	if (m > MAX_MESSAGE_LENGTH){
		perror("Cannot send message larger than the maximum message length, which is 200 bytes");
	}
	write_buffer[m]='\0';
	int i=0;
	ssize_t message_size;
	ssize_t received_size;
	//write out the length of the string read in plus 1 for the null terminator at the end
	message_size=send(sockfd,write_buffer,strlen(write_buffer)+1,0);
	recv(sockfd,read_buffer,strlen(write_buffer)+1,0);
	int j=strlen(read_buffer);
	read_buffer[j]='\0';
	int z=0;
	printf("Reversed server response: %s\n",read_buffer);
}



int main(int argc, char*argv[]){
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int cfd;
	socklen_t socklen;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family=AF_UNSPEC; //can accept IPV4 or IPV6
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_next=NULL; 
	hints.ai_canonname=NULL;
	hints.ai_addr=NULL;
	hints.ai_flags=AI_NUMERICSERV;
	char addbuff[NI_MAXSERV+NI_MAXHOST+10];
	char write_buffer[MAX_MESSAGE_LENGTH];
	char read_buffer[MAX_MESSAGE_LENGTH];
	bzero(write_buffer,sizeof(write_buffer));
	bzero(read_buffer,sizeof(read_buffer));
	if (argc<2){
		printf("Failed to give hostname for client");
		exit(EXIT_FAILURE);
	} 
	if ((getaddrinfo(argv[1],PORT_NUM,&hints,&result))!=0){
		printf("You did not provide a legitimate host name for the client socket to search through addresses to connect to");
		exit(EXIT_FAILURE);
	}
 	for (rp=result;rp;rp=rp->ai_next){
		cfd=socket(rp->ai_family,rp->ai_socktype, rp->ai_protocol);
		if (cfd==-1){
			continue;
		}
		if (connect(cfd,rp->ai_addr,rp->ai_addrlen)!=-1){
			break;
		}
	}
	chat_function(cfd,write_buffer,read_buffer);

	if (rp==NULL){
		printf("Could not connect socket to any address");
		exit(EXIT_FAILURE);
	}
	close(cfd);
}