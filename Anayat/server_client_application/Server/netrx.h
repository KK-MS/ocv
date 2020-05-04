#ifndef NETRX_H
#define NETRX_H
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

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
  int sock_output;

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

} netrx;
#endif