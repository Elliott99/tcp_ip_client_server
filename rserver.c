#include <netdb.h>
#define BACKLOG 20
#include "server.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_MESSAGE_LENGTH 200


int main(){
	int cfd,afd;
	char service[NI_MAXSERV];
	//hostname will be the name of the IP address
	char host[NI_MAXHOST];
	//hints is used to set certain settings within each of the addrinfo structs in the linkedlist that will be spawned by getaddrinfo 
	struct addrinfo hints;
	//this is used for looping through possible addrinfo structus
	struct addrinfo *result, *rp;
	//this struct will store the information of the peer socket (service name/host name) that will connect to our client
	struct sockaddr_storage claddr;
	//combined host + service name with padding of 10 bits
	char addrstr[NI_MAXHOST+NI_MAXSERV+10];
	memset(&hints,0,sizeof(struct addrinfo));
	//socklen is the size of the socket
	socklen_t socklen;
	//AF_UNSPEC means that we can use an unspecified IP protocl: IPV4 or IPV6
	hints.ai_family=AF_UNSPEC; 
	//spawn addresses for stream, not datagram sockets
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_next=NULL;
	hints.ai_canonname=NULL;
	hints.ai_addr=NULL;
	//Passive: we wait for someone to join with our socket, numeric serv: use the numeric service name name
	hints.ai_flags=AI_PASSIVE | AI_NUMERICSERV;
	//getadrrinfo: 0 is success, takes in as arguments, NULL(?) our port number, references to the hints and result addrinfo structs
	//getaddrinfo generates a linked list of addrinfo structs for the specified host name/service name\
	//A null first parameter here, in combination with the AI PASSIVE flag set in hints, indicates this will be a server that will listen and wait for connections
	//in this case, result is the head of the linkedlist, hints is the hints thing that sets csome conditions
	if ((getaddrinfo(NULL,PORT_NUM,&hints,&result))!=0){
		printf("Failed to get result pointer of address structs");
		exit(EXIT_FAILURE);
	}
	//loop through possible addrinfo structs we can successfully create a socket at. Create socket at every possible struct. If we are successful
	//in binding then quit. bind takes as arguments the socket's file descriptor, the address of the socket, and the lenth of the socket's address
	//socket takes as arguments the addrinfo structu's ai_family (IPV4/6), socketpye (stream socket), and the protocol?
	for (rp=result;rp;rp!=NULL){
		cfd=socket(rp->ai_family,rp->ai_socktype, rp->ai_protocol);
		if (cfd==-1){
			continue;
		}
		//rp->ai_addr could be a pointer to a sockaddr_in or sockaddr_in6
		//once we've successfully bound our socket to a sockaddrinfo struct, break out of the loop
		if (bind(cfd,rp->ai_addr, rp->ai_addrlen)==0){
			break;
		}
	} 
	if (rp==NULL){

		printf("Reached end of address list without finding suitable socket address space");
		exit(EXIT_FAILURE);
	}
	//free memory that was alloccated to result sockaddrinfo pointer which we created to let us loop through the list of possible addresses
	freeaddrinfo(result);
	if (listen(cfd,BACKLOG)==0){
		printf("Server listening....\n");
	}
	for (;;){
		socklen=sizeof(struct sockaddr);
		afd=accept(cfd,(struct sockaddr*) &claddr,&socklen);
		if (afd==-1){
			perror("Accept failed");
		}
		//arguments: a pointer to a referce to a struct that contains the host and service info of the connecting socket, the length of the connecting socket
		//host and serv are caller-allocated buffers that hold the host/service name we got from the first argument
		//this will populate our host and service buffers 
		getnameinfo((struct sockaddr*) &claddr,socklen,
			host,NI_MAXHOST,service,NI_MAXSERV,0);
		//format a string that contains both of the service and host names
		snprintf(addrstr,NI_MAXSERV+NI_MAXHOST+10, "Connection received from: (%s, %s)", host,service);	
		printf("%s\n",addrstr);
	
	}
	close(afd);
	close(cfd);
}