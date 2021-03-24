#include <netdb.h>
#include "server.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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
	//Command line argument must be a hostname for the client
	if (argc<2){
		printf("Failed to give hostname for client");
		exit(EXIT_FAILURE);
	} 
	//call getaddrinfo to attempt to search for addresses that our socket can eventually connect to
	//should use the "localhost" host name here as a command line address
	if ((getaddrinfo(argv[1],PORT_NUM,&hints,&result))!=0){
		printf("You did not provide a legitimate host name for the client socket to search through addresses to connect to");
		exit(EXIT_FAILURE);
	}
	//loop through the possible addresses generated from getaddrinfo until we successfully can connect to one
	//bind() is not needed for clients: we are interested only in connceting to a different endpoint, and the kernel will handle,
	//through the routing table, establishing an IP address and port number for our client
 	for (rp=result;rp;rp=rp->ai_next){
		cfd=socket(rp->ai_family,rp->ai_socktype, rp->ai_protocol);
		if (cfd==-1){
			continue;
		}
		if (connect(cfd,rp->ai_addr,rp->ai_addrlen)!=-1){
			int error = 0;
			socklen_t len = sizeof (error);
			int retval = getsockopt (cfd, SOL_SOCKET, SO_ERROR, &error, &len);
			if (retval==0){
				printf("Socket successfully connected to client from server\n");
			}
			break;
		}
	}
	if (rp==NULL){
		printf("Could not connect socket to any address");
		exit(EXIT_FAILURE);
	}
	close(cfd);
}