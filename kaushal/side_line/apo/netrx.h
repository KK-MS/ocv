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

#include "packet.h"

// MACROS
// Possible to set input resolution (must be supported by the DCU)
#define WIDTH            (1280u) // width of DDR buffer in pixels
#define HEIGHT           (720u)  // height of DDR buffer in pixels
#define DDR_BUFFER_CNT   (3u)    // number of DDR buffers per ISP stream

#define DDR_BUF_SIZE (WIDTH * HEIGHT * DDR_BUFFER_CNT)

#define FRAME_WIDTH 1280         // For processing the frame

// Namespace
using namespace std;

// STRUCTURES

// NETRX: Application main structure
typedef struct netrx {

  // Request type
  PACKET stPacket;

  // GT Lane packet
  GT_LANE_PACKET stGtLanePacket;

  int sock_desc;
  int sock_desc_gt;
  int sock_desc_gt_bridge;

  char *serverIP;
  char *serverIP_nxp;
  char *serverIP_gt_bridge;

  int port;
  int port_gt;
  int port_gt_bridge;

  int protocol;
  int protocol_gt;
  int protocol_gt_bridge;

  int bind_sock_desc;
  int bind_sock_desc_gt;
  int bind_sock_desc_gt_bridge;
  
  // Buffer handling variables
  char *ptr_req_buf;  // pointer to buffer where request is received
  char *ptr_send_buf; // Send buffer
  int size_send_buf;  // Size of data to be transmitted.

} netrx;

#endif /* NETRX_H */
