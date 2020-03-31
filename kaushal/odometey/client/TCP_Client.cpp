/*
   Create a TCP socket
   */
#include<stdio.h>
#include <stdlib.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include<io.h>

#include <fcntl.h>
#include <errno.h>

#include "packet.h"
#include "netrx.h"


int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in client;
    
	memset(&client, 0, sizeof(client));
  
	int send_size;
	int read_size;
  
	printf("\nInitialising Winsock...");
  
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");
	fflush(stdout);

	//Create a socket
	if((sock = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}
	printf("Socket created.\n");
	fflush(stdout);

	// assign the address to scoket
	client.sin_addr.s_addr = inet_addr("127.0.0.1");
	client.sin_family = AF_INET;
	client.sin_port = htons( 8888 );
  
	//Connect to remote server
	if (connect(sock, (struct sockaddr *)&client , sizeof(client)) < 0)
	{
		puts("connect error");
		fflush(stdout);
		return 1;
	}

	puts("\nConnected.");
	fflush(stdout);

	/*Receive METADATA from Server */
	
	netrx ptrCliNet;
	
	// Packet structure define
    PACKET *ptr_metadata = (PACKET *) &(ptrCliNet.stPacket);	
	
	FILE* file = fopen("Mockup_Data_CM_TS.csv", "a");

	fprintf(file,"Time(S), Latitude, Longitude, CM_D2TS\n");
	
	fclose(file);
	
    while(1)
    {
		file = fopen("Mockup_Data_CM_TS.csv", "a");

	    //fprintf(file,"frame, D2L(mm)\n");
	 
	    //  APO -> NXP_server : CM Data
        read_size = recv(sock, (char *)ptr_metadata, sizeof(PACKET), 0);
        if (read_size < 0) {
			printf("***Error in Receiving MetaData: [%d] : %s\n", errno, strerror(errno));
			return -1;
		}

		if (read_size != sizeof(PACKET)) {
			printf("***Error: RX METADATA PACKET got %d\n", read_size);
			// TODO: log the error and request again for metadata
		}
		
		printf("read_size:%d\n", read_size);

		// Display the TIME + CM_GPS + TS + D2TS Data
		printf("\nTimeL: %f\n",ptr_metadata->u4_timestampL);
		printf("lat: %f\n", ptr_metadata->u4_ins_latitude);
		printf("lon: %f\n", ptr_metadata->u4_ins_longitude);
		printf("CM_D2L: %f\n", ptr_metadata->u4_ins_TSign_ds);	
	
		// Write the process METADATA in .csv file
		fprintf(file,"%f, %f, %f, %f\n", ptr_metadata->u4_timestampL, ptr_metadata->u4_ins_latitude, ptr_metadata->u4_ins_longitude, ptr_metadata->u4_ins_TSign_ds);
		
		fclose(file);
    }



  closesocket(sock);
  WSACleanup();
  return 0;
}