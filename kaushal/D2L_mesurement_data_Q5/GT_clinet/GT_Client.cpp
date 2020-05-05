#include<stdio.h>
#include <iostream>
#include <stdlib.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include<io.h>
#include <fstream>
#include <sstream>

#include <fcntl.h>
#include <errno.h>

#include "packet.h"
#include "netrx.h"

using namespace std;

/*
 ** Description
 **
 ** GT_Client information send to Host sever.
 ** for process the frame to create the ROI for Find D2L. 
 */

int main(int argc , char *argv[])
{
  WSADATA wsa;
  SOCKET sock;
  struct sockaddr_in client;
  int send_size;
  int read_size;
  
  memset(&client, 0, sizeof(client));

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
  client.sin_port = htons( 8889 );

  //Connect to remote server
  if (connect(sock, (struct sockaddr *)&client , sizeof(client)) < 0)
  {
    puts("connect error");
    fflush(stdout);
    return 1;
  }

  puts("\nConnected.");
  fflush(stdout);

  // structure  define
  netrx ptrCliNet;

  // Packet structure define
  PACKET *ptr_metadata = (PACKET *) &(ptrCliNet.stPacket);
 
 // varibles fro GT_METADA csv file reading for create the ROI
  int row_count = 0;
  string row;
  bool Skip = true;
  
  float GT_D2L; // in cm
  
  // Open the GT_METADA csv file for create the ROI for D2L processing in frame
  char *csv_File_name = argv[1];
  ifstream  data(csv_File_name);
  
  // Skip the header line or first line of file
  getline(data, row);  

  // read the GT_data line by line and send to host server
  while (getline(data, row))
  {
    // get the needed GT_METDADA from csv file for process the frame in sequence
	row_count += 1;

	std::stringstream  rowStream(row);
	string        cell;
	int column_count = 0;

	// extract the words from row in to columns
	while (getline(rowStream, cell, ',')){

		column_count += 1;

		// You have a cell of GT_D2L
		if (column_count == 5) {

			// check the cell, if empty then replace with 0
			if (cell.empty() && Skip)
			{
				cell = "0";
				Skip = false;
			}
			GT_D2L = stof(cell);
			std::cout << endl << cell << " row " << row_count << " column " << column_count << "  Dist: " << GT_D2L;
		}
	}
	ptr_metadata->u4_ins_cm_d2l = GT_D2L;
	
	// send GT_Data to Host_server
	send_size = send(sock, (char*)ptr_metadata, sizeof(PACKET), 0);
	if (send_size == -1) {
		printf("***Error in sending Request for MetaData: [%d] : %s\n"
			, errno, strerror(errno));
		return -1;
	}
	printf("send_size:%d\n", send_size);
 } // loop

  closesocket(sock);
  WSACleanup();
  return 0;
}
