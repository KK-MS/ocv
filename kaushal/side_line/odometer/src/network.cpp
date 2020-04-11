
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#pragma warning(disable:4996) 

#include "packet.h"
#include "netrx.h"


/*
 ** Functions Details
 **
 ** create_server
 ** accept_client
 */

 /*
  ** create_server
  **
  ** Create the network for listen the  Clients
  */
int create_server(netrx* ptrCliNet)
{
	WSADATA wsa;
	//char* serverIP = ptrCliNet->serverIP;
	//int port = ptrCliNet->port;
	int socket_desc;

	struct sockaddr_in server; 

	//printf("\nNXP_Server Creating..%s:%d\n", serverIP, port);
	printf("\nInitialising Winsock...and b4 WSAStartup\n");

	fflush(stdout);

	memset((char*)& wsa, 0, sizeof(wsa));

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");
	fflush(stdout);

	// Socket Create
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		perror("Could not create socket");
		return -1;
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(8889);
	//server.sin_addr.s_addr = inet_addr(serverIP);
	//server.sin_port = htons(port);

	//Bind
	if (::bind(socket_desc, (struct sockaddr*) & server, sizeof(server)) < 0) {
		perror("bind failed");
		return -1;
	}
	printf("bind done");

	ptrCliNet->bind_sock_desc_input_data = socket_desc;

	//Listen // TODO Error check
	listen(socket_desc, 3);

	return socket_desc;


}/*
 ** accept_client
 **
 ** Listen the Multiple clients and established conenction.
 **/
int accept_client(netrx* ptrCliNet)
{
	int socket_desc_input_data = ptrCliNet->bind_sock_desc_input_data;
	int GT_bridge_socket;
	struct sockaddr_in client;
	int length = sizeof(struct sockaddr_in);
	int read_size;

	//Accept the  incoming connection
	puts("\nHost Server Waiting for incoming connections...");

	if ((GT_bridge_socket = accept(socket_desc_input_data, (struct sockaddr*) & client, & length)) > 0) {
		printf("Connection accepted.\n");
	}

	fflush(stdout);

	if (GT_bridge_socket < 0) {
		perror("\nAccept Failed");
		return -1;
	}

	// Getting corrected METADATA from GT_Server
	read_size = recv(GT_bridge_socket, (char*)& ptrCliNet->stPacket, sizeof(PACKET), 0);
	if (read_size < 0) {
		printf("***Error in Receiving Client message: [%d] : %s\n"
			, errno, strerror(errno));
		return -1;
	}

	printf("read_size:%d\n", read_size);
	printf("read_Data:%s\n", (char*)& ptrCliNet->stPacket);

	// Receive first data from the client.
	// Client will info, who it is.
	// Based on it, assign it to the variable
	if (strcmp((char*)& ptrCliNet->stPacket, "APO") == 0) {

		ptrCliNet->sock_desc_input_data = GT_bridge_socket;
		printf("\n APO_Client Connected.:%d\n", GT_bridge_socket);

	}
	else if (strcmp((char*)& ptrCliNet->stPacket, "GTC") == 0) {

		ptrCliNet->sock_desc_gt = GT_bridge_socket;
		printf("\n GT_Python Client Connected.:%d\n", GT_bridge_socket);
	}
	else if (strcmp((char*)& ptrCliNet->stPacket, "APP") == 0) {

		ptrCliNet->sock_desc_app = GT_bridge_socket;
		printf("\n Application_client Connected.:%d\n", GT_bridge_socket);

	}
	else {

		printf("***Error: Unknown Client.\n");
		exit(0);
	}

	return GT_bridge_socket;
}

