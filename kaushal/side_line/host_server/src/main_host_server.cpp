/*
    Live Server on port 8888
*/

#include<io.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <math.h>
#include <signal.h>
#include <inttypes.h>
#include <fcntl.h>
#include <errno.h>

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#pragma warning(disable:4996) 

#include "opencv2/opencv.hpp"

#include "packet.h"
#include "netrx.h"

// Possible to set input resolution (must be supported by the DCU)
#define RECEIVE_BUF_SIZE    (1448)
#define CM_VDS_FRAME_WIDTH  (640)
#define CM_VDS_FRAME_HEIGHT (480)

#define CM_FRAME_SIZE  (CM_VDS_FRAME_WIDTH * CM_VDS_FRAME_HEIGHT * 3)

using namespace std;
using namespace cv;

/* Functions declaration */
int create_server(netrx* ptrCliNet);

int accept_client(netrx* ptrCliNet);

int run_app(netrx* ptrCliNet);


void help_msg()
{
	printf("***Error!! in arguments\n");
	printf("Usage: <client> <NXP_serverIP> <NXP_portNo> <NXP_SERVERIP: GT_client> <NXP_PORTNO: GT_Client> \
      <Frame_Pr_StartX> <Frame_Pr_StartY> <Frame_Pr_endY> <Frame_Pr_Pixel_Count> \
      <Frame_Pr_color_threshold> <-v : [For receive the CM_Video Frame]>\n");
	printf("./side_lane_server_app.elf 10.5.11.66 8889 10.5.11.88 8890 495 380 380 15 250 -v\n");
}

/*
 ** connect_clients
 **
 ** Connect the APO & GT Client
 */
int connect_clients(netrx* ptrCliNet)
{
	int socket_desc;

	// accept vds and metadata client
	socket_desc = accept_client(ptrCliNet);
	if (socket_desc < 0) {
		printf("Error in process_request: no.:[%d] %s\n", errno, strerror(errno));
		return -1;
	}

	// accept GT client
	socket_desc = accept_client(ptrCliNet);
	if (socket_desc < 0) {
		printf("Error in process_request: no.:[%d] %s\n", errno, strerror(errno));
		return -1;
	}

	// TODO accept output client
	socket_desc = accept_client(ptrCliNet);
	if(socket_desc < 0) {
	  printf("Error in process_request: no.:[%d] %s\n", errno, strerror(errno));
	  return -1;
	}

	//return 0;
	return socket_desc;
}

/*
 ** side_lane_application
 **
 ** connect and Receive the frames & MAPDATA from CM_APO Client
 */
int side_lane_application(netrx* ptrCliNet)
{
	int socket_desc;
	int ret;

	// Create and Start the server to listen
	if ((ret = create_server(ptrCliNet)) < 0) {
		return ret;
	}

	// Connect to the multile clients
	if ((ret = connect_clients(ptrCliNet)) < 0) {
		return ret;
	}

	// receiving the Frames & MAPDATA from VD_GT_Server
	// loops until terminated or error in processing

	printf("\nRun Application...\n");
	
	//run_app(ptrCliNet);
	if ((ret = run_app(ptrCliNet)) < 0) {
		return -1;
	}

	printf("\n@END: Received new Position...Status:%d\n", ret);

	return 0;
}


/******************************** MAIN FUNCTION *********************************************/

int main(int argc , char *argv[])
{
    
	int tot_arg = argc;

	char serverIP;
	int port;

	char* serverIP_gt;
	int port_gt;

	int process_start_x;
	int process_start_y;
	int Process_end_y;
	int process_lanepixel_count;
	int process_lanecolor_threshold;

	char* video_filename;

#if 0
	if (tot_arg < 9) {
		help_msg();
		return -1;
	}
#endif

	netrx cliNet; // TODO use malloc to create netrx object.

	// TODO Error check of input parameters

	//serverIP = "127.0.0.1";  // argv[1];
	//port = 8888; // atoi(argv[2]);

	//serverIP_gt = argv[3];
	//port_gt = atoi(argv[4]);

	process_start_x = 420;			    // atoi(argv[5]);
	process_start_y = 512,			    // atoi(argv[6]);
	Process_end_y = 512;                // atoi(argv[7]);
	process_lanepixel_count = 15;       // atoi(argv[8]);
	process_lanecolor_threshold = 250;  // atoi(argv[9]);

	//video_filename = argv[11];

	//cliNet.serverIP = serverIP;
	//cliNet.serverIP_gt = serverIP_gt;
	//cliNet.port = port;
	//cliNet.port_gt = port_gt;

	//cliNet.istart_x = process_start_x;
	//cliNet.istart_y = process_start_y;
	//cliNet.iend_y = Process_end_y;
	//cliNet.iLanePixelThreshold = process_lanepixel_count;
	//cliNet.iLaneColorUpperThreshold = process_lanecolor_threshold;

	//cliNet.vd_filename = video_filename;

	// Start the application
	side_lane_application(&cliNet);




    //closesocket(socket_desc);
    //WSACleanup();

    return 0;
}