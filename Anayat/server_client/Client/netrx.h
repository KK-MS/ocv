#ifndef NETRX_H
#define NETRX_H

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

//#include "opencv2/opencv.hpp"

#include "packet.h"
// Namespace
using namespace std;

// STRUCTURES

// NETRX: Application main structure
typedef struct netrx {

  // Request type
  PACKET stPacket;


  int sock_desc;
  int sock_desc_app;
  int sock_desc_input_data;

  char *serverIP;
  char *serverIP_input_data;

  int port;
  int port_input_data;

  int protocol;
  int protocol_input_data;

  int bind_sock_desc;
  int bind_sock_desc_input_data;

  // At Client side: Frames Receive mode selection -> 1. VD_GT 2. CM
  int server_selection;

  // Buffer handling variables
  char *ptr_req_buf;  // pointer to buffer where request is received
  char *ptr_send_buf; // Send buffer
  int size_send_buf;  // Size of data to be transmitted.

  int data_index;

  FILE *file;
  FILE *lane_file;

  // Request related variables
  char *filename;
  char *vd_filename;
  char *img_filename;

  // Variables for process the frames (D2L)
  int istart_x;
  int istart_y;
  int iend_y;

  // lane threshold parameters
  int iLanePixelCount = 0;
  int iLanePixelThreshold;
  int iLaneColorUpperThreshold;


} netrx;
#endif