#include <netdb.h>
#define BACKLOG 20
#include "server.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_MESSAGE_LENGTH 200


void reverser_response(int sockfd,char *write_buff,char *read_buff){
	ssize_t received;
	received=recv(sockfd,read_buff,sizeof(read_buff),0);
	printf("Server received %zu bytes\n",received);
	char *reversed;
	int l=strlen(read_buff);
	//get string length of word we received 
	//allocate enough space for the reversed word + a null terminator at the end
	reversed=(char*)(malloc(sizeof(char) * (l+1)));
	int i=0;
	int length=l+1;
	//populate the reversed string literal with characters in opposite order of input read in through read buff
	//populate write buff with this new reversed string
	while (i!=l){
		printf("%c\n",read_buff[i]);
		reversed[(l-1)-i]=read_buff[i];
		printf("Reversed %d is %c\n",(l-1)-i,reversed[(l-1)-i]);
		write_buff[(l-1)-i]=reversed[(l-1-i)];
		i++;
	}
	write_buff[l]='\0';
	//write out all characters, plus 1 for the null terminator back to the client
	ssize_t sent=send(sockfd,write_buff,strlen(write_buff)+1,0);
	printf("Server sent back message of this size: %zu\n",sent);
}



int main(){
	int cfd,afd;
	//I'm pretty sure service will be the port name
	char service[NI_MAXSERV];
	//hostname will be the name of the IP address
	char host[NI_MAXHOST];
	char read_buff[200];
	char write_buff[200];
	//I rmemember hints is used to set certain settings within the struct addrinfo result we create
	struct addrinfo hints;
	//this is used for looping through possible addrinfo structs
	struct addrinfo *result, *rp;
	//I think this stores the address of the client that connect with us
	struct sockaddr_storage claddr;
	//combined host + service name with padding of 10 bits
	char addrstr[NI_MAXHOST+NI_MAXSERV+10];
	memset(&hints,0,sizeof(struct addrinfo));
	bzero(read_buff,sizeof(read_buff));
	bzero(write_buff,sizeof(write_buff));
	//socklen is the size of the socket
	socklen_t socklen;
	//AF_UNSPEC means that we can use an unspecified IP protocl: IPV4 or IPV6
	hints.ai_family=AF_UNSPEC; 
	//stream socket
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_next=NULL;
	hints.ai_canonname=NULL;
	hints.ai_addr=NULL;
	//Passive: we wait for someone to join with our socket, numeric serv: use the numeric host name
	hints.ai_flags=AI_PASSIVE | AI_NUMERICSERV;
	//getadrrinfo: 0 is success, takes in as arguments, NULL(?) our port number, references to the hints and result addrinfo structs
	//actually getaddrinfo generates a linked list of addrinfo structs for the specified host name/service name
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
		if (bind(cfd,rp->ai_addr, rp->ai_addrlen)==0){
			break;
		}
	} 
	if (rp==NULL){

		printf("Reached end of address list without finding suitable socket address space");
		exit(EXIT_FAILURE);
	}
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
		getnameinfo((struct sockaddr*) &claddr,socklen,
			host,NI_MAXHOST,service,NI_MAXSERV,0);
		snprintf(addrstr,NI_MAXSERV+NI_MAXHOST+10, "Connection received from: (%s, %s)", host,service);	
		printf("%s\n",addrstr);
		reverser_response(afd,write_buff,read_buff);
	
	}
	close(afd);
	close(cfd);
}