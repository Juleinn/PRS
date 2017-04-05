#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h> //structures sockaddr_in
#include <arpa/inet.h> //inet_addr
#include <sys/types.h> //sendto
#include <sys/socket.h> // sendto
#include <unistd.h> //close
#include <string.h> //strlen
#define RCVSIZE 1024

int main(int argc, char **argv)
{
	printf("\n------------------SERVER------------------ \n \n");
	socklen_t adresslen=sizeof(struct sockaddr_in);
	char buf[RCVSIZE];
	int sizeData=0;
	int valid = 1;
	
	//Create structure
	struct sockaddr_in adressServer;
	struct sockaddr_in adressClient;
	
	//Port used to initiate connections
	int port= atoi(argv[1]);


	 //Socket d'écoute
	int desc= socket(AF_INET, SOCK_DGRAM, 0);
	
	
	//Error socket creation
	if (desc < 0) {
		perror("cannot create socket\n");
		return -1;
	}
	
	setsockopt(desc, SOL_SOCKET, SO_REUSEADDR, &valid, sizeof(int));
	
	//Configure the struct associated with socket
	adressServer.sin_family= AF_INET;
	adressServer.sin_port= htons(port);
	adressServer.sin_addr.s_addr= htonl(INADDR_ANY);
	
	//Bind socket-descripter
	if (bind(desc, (struct sockaddr*) &adressServer, sizeof(adressServer)) == -1) {
		perror("Bind fail\n");
		close(desc);
		return -1;
	}
	adresslen=sizeof(adressClient);
	
	//Infinite loop for server listening
	while(1) {
		sleep(2);
		
		//Clean buffer
		memset(buf,0,RCVSIZE);
		
		//Receive message from client
		printf("Waiting for packet \n");
		sizeData=recvfrom(desc,buf,sizeof(buf),0,(struct sockaddr*) &adressClient,&adresslen);
		if(sizeData<0)
		{
			printf("Error recvfrom() \n");
			return -1;
		}
		/*------------------------------------------------------------*/
		/*------------------------SYN RECEIVED------------------------*/
		/*------------------------------------------------------------*/
		
		else if(strcmp(buf,"SYN")==0)
		{
			printf("Received %s from client (%d bytes)\n \n", buf,sizeData);
			char portString[5];
			snprintf(portString,10,"%d",port);
			
			//Send SYN-ACK to client et and send port adress
			strcpy(buf,"SYN-ACK_");
			strcat(buf,portString);
			
			if((sendto(desc,buf,sizeof(buf),0,(struct sockaddr*) &adressClient,adresslen))==-1)
			{
				printf("Erreur sendto() SYN-ACK \n");
				return -1;
			}
			else
			{
				printf("Successed to send %s to client \n",buf);
			}
		}
		/*------------------------------------------------------------*/
		/*------------------------ACK RECEIVED------------------------*/
		/*------------------------------------------------------------*/
		else if(strcmp(buf,"ACK")==0)
		{
			printf("Received %s from client (%d bytes) \n \n", buf, sizeData);
			
			
			/*--------------------------------------------------------*/
			/*-------------------------FORK---------------------------*/
			/*--------------------------------------------------------*/

			int pid=fork();
			//ERROR
			if(pid==-1)
			{
				printf("Erreur fork \n");
			}
			
			/*--------------------------------------------------------*/
			/*--------------------------------------------------------*/
			/*--------------------CHILD PROCESS-----------------------*/
			/*--------------------------------------------------------*/
			/*--------------------------------------------------------*/
			
			else if (pid==0)
			{
				printf("Je suis le fils  \n");
			}
				
			/*--------------------------------------------------------*/
			/*--------------------------------------------------------*/
			/*--------------------FATHER PROCESS----------------------*/
			/*--------------------------------------------------------*/
			/*--------------------------------------------------------*/
			else
			{
				printf("Je suis le père \n");
			}
			
			
			/*--------------------------------------------------------*/
			/*--------------------------------------------------------*/		
		}
	}
}
