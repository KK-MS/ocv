#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#pragma warning(disable:4996) 

#include "packet.h"
#include "netrx.h"

// create_server
  
int create_server(netrx* ptrCliNet)
{
	WSADATA wsa;
	//char* serverIP = ptrCliNet->serverIP;
	//int port = ptrCliNet->port;
	int socket_desc;

	struct sockaddr_in server;

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

	//Bind
	if (::bind(socket_desc, (struct sockaddr*) & server, sizeof(server)) < 0) {
		perror("bind failed");
		return -1;
	}
	printf("bind done");

	ptrCliNet->bind_sock_desc_input_data = socket_desc;

	//Listen // TODO Error check
	listen(socket_desc, 3);

	return 0;
}

//accept_client
 
int accept_client(netrx* ptrCliNet)
{
	int socket_desc_input_data = ptrCliNet->bind_sock_desc_input_data;
	int Application_socket;
	struct sockaddr_in client;
	int length = sizeof(struct sockaddr_in);
	int read_size;

	//Accept the  incoming connection
	puts("\nHost Server Waiting for incoming connections...");

	if ((Application_socket = accept(socket_desc_input_data, (struct sockaddr*) & client, &length)) > 0) {
		printf("Connection accepted.\n");
	}

	ptrCliNet->sock_desc_app = Application_socket;

	fflush(stdout);

	if (Application_socket < 0) {
		perror("\nAccept Failed");
		return -1;
	}

	return 0;
}

int send_packet(netrx* ptrclient)
{
	PACKET *ptr_metadata;
	int send_size;
	int process_start_x;
	int process_start_y;
	int Process_start_width;
	int Process_start_height;
	cout << "Enter the value of top left x coordinate:";
	cin >> process_start_x;
	ptrclient->stPacket.x_coordinate = process_start_x;
	cout << "Enter the value of top left y coordinate:";
	cin >> process_start_y;
	ptrclient->stPacket.y_coordinate = process_start_y;
	cout << "Enter the value of rectangle width:";
	cin >> Process_start_width;
	ptrclient->stPacket.width = Process_start_width;
	cout << ptrclient->stPacket.width << endl;
	cout << "Enter the value of rectangle height:";
	cin >> Process_start_height;
	ptrclient->stPacket.height = Process_start_height;

	ptr_metadata = (PACKET*) & (ptrclient->stPacket);
	send_size = send(ptrclient->sock_desc_app, (char*)ptr_metadata, sizeof(PACKET), 0);
	if (send_size == -1) {
		printf("***Error in sending Request for MetaData: [%d] : %s\n"
			, errno, strerror(errno));
		return -1;
	}
	printf("send_size:%d\n", send_size);
	printf("\nSent ROI x_coordinate value: %d\n", ptr_metadata->x_coordinate);
	printf("\nSent ROI y_coordinate value: %d\n", ptr_metadata->y_coordinate);
	printf("\nSent ROI width: %d\n", ptr_metadata->width);
	printf("\nSent ROI height: %d\n", ptr_metadata->height);

	return 0;
}

int main(int argc, char* argv[])
{
	netrx client;
	
	int send_size;
	create_server(&client);
	accept_client(&client);
	while (1)
	{
		send_packet(&client);
	
	}

	return 0;
	
}


