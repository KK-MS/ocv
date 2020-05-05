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

// TODO:
// description:
//
// Client application to receive the corrected position.
// E.g. in ground truth project: role of Genesys
int main(int argc , char *argv[])
{
  WSADATA wsa;
  SOCKET sock;
  struct sockaddr_in client;

  memset(&client, 0, sizeof(client));

  int send_size;
  int read_size;
  char info[] = "APP";

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

#if 0

  //send the CM_METADAT to NXP_Server fro Frame processing
  send_size = send(sock, info, sizeof(info) , 0);

  if(send_size == -1){
    printf("\n***Error in sending request for Lane information: [%d] : %s, [%d]\n", errno, strerror(errno), send_size);
    return -1;
  }

  printf("send Size: %d\n", send_size);
#endif

  /*Receive METADATA from Server */

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
	
	// send CM Data to GT_server
	send_size = send(sock, (char*)ptr_metadata, sizeof(PACKET), 0);
	if (send_size == -1) {
		printf("***Error in sending Request for MetaData: [%d] : %s\n"
			, errno, strerror(errno));
		return -1;
	}
	printf("send_size:%d\n", send_size);
	
#if 0
    //  Host server to the odometry
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
    printf("CM_D2L: %f\n", ptr_metadata->u4_ins_cm_d2l);
    printf("New_lat: %f\n", ptr_metadata->u4_out_odo_latitude);
    printf("New_lon: %f\n", ptr_metadata->u4_out_odo_longitude);
    printf("New_lon: %f\n", ptr_metadata->u4_odo_distance);

    // Write the process METADATA in .csv file
    fprintf(file,"%f, %f, %f, %f, %f, %f, %f\n", ptr_metadata->u4_timestampL, ptr_metadata->u4_ins_latitude, ptr_metadata->u4_ins_longitude, ptr_metadata->u4_ins_cm_d2l, ptr_metadata->u4_odo_distance, ptr_metadata->u4_out_odo_latitude, ptr_metadata->u4_out_odo_longitude);
#endif

 }



  closesocket(sock);
  WSACleanup();
  return 0;
}
