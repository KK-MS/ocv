/*****************************************************************************
*
* A Drive Living Lab Confidential Proprietary
*
* Copyright (c) 2019 A Drive Living Lab
* All Rights Reserved
*
*****************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY A DRIVE LIVING LAB "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL A DRIVE LIVING LAB OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************/


#ifndef NETRX_H
#define NETRX_H

//#include <arpa/inet.h>

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "opencv2/opencv.hpp"

#include "packet.h"

// MACROS
// Possible to set input resolution (must be supported by the DCU)
#define WIDTH            (1280u) // width of DDR buffer in pixels
#define HEIGHT           (720u)  // height of DDR buffer in pixels
#define DDR_BUFFER_CNT   (3u)    // number of DDR buffers per ISP stream

#define DDR_BUF_SIZE (WIDTH * HEIGHT * DDR_BUFFER_CNT)

#define FRAME_WIDTH 1280         // For processing the frame

#define CM_VDS_FRAME_WIDTH  (640)
#define CM_VDS_FRAME_HEIGHT (480)

#define CM_FRAME_SIZE  (CM_VDS_FRAME_WIDTH * CM_VDS_FRAME_HEIGHT * 3)

// Namespace
using namespace std;
using namespace cv;

// STRUCTURES

// NETRX: Application main structure
typedef struct netrx {

  // Request type
  PACKET stPacket;

  // GT Lane packet
  GT_LANE_PACKET stGtLanePacket;

  int sock_desc;
  int sock_desc_gt;
  int sock_desc_app;
  int sock_desc_input_data;

  char *serverIP;
  char *serverIP_gt;
  char *serverIP_input_data;

  int port;
  int port_gt;
  int port_input_data;

  int protocol;
  int protocol_gt;
  int protocol_input_data;

  int bind_sock_desc;
  int bind_sock_desc_gt;
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

  // UDP
  struct sockaddr_in sock_remote;
  struct sockaddr_in sock_remote_gt;
  unsigned int sock_remote_length;
  unsigned int sock_remote_length_gt;


  // FRAMEDATA variables
  int width = CM_VDS_FRAME_WIDTH;
  int height = CM_VDS_FRAME_HEIGHT;
  int ImgLen = CM_FRAME_SIZE;

  //char* img; 
  char* img = (char*)malloc(ImgLen);

  // Customize
  Mat mat_logo;

  // OUTPUT
  VideoWriter wrOutVideo;

} netrx;

#endif /* NETRX_H */