/*****************************************************************************
*
* A Drive Living Lab Confidential Proprietary
*
* Copyright (c) 2019 A Drive Living lab
* All Rights Reserved
*
*****************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY A DRIVE LIVING LAB "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL A DRIVE LIVING LAB OR ITS CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<io.h>
#include <fcntl.h>
#include <errno.h>

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "packet.h"
#include "netrx.h"

using namespace std;

/*
 ** Functions Declaration.
 */
int network_create(netrx *ptrCliNet);

int network_listen(netrx *ptrCliNet);

int gt_info_send(netrx *ptrCliNet);

/*
 ** gt_server_embedded
 **
 ** Network Create & Conenct to host clinet 
 ** Receive the IMU DATA from client
 ** receive the request for require data
 ** Process the request
 ** Send the SL/TS data to client
 */
int gt_server_embedded(netrx *cliNet)
{
  int ret_val;
  int socket_desc;

  ret_val = network_create(cliNet);
  if(ret_val < 0) {
    printf("\nError in network create:[%d] %s\n", errno, strerror(errno));
    return -1;
  }

  socket_desc = network_listen(cliNet);
  if(socket_desc < 0) {
    printf("Error in process_request: no.:[%d] %s\n", errno, strerror(errno));
    return -1;
  }

  // receiving the IMU Data, Process it and send the Require GT info to client 
  ret_val = gt_info_send(cliNet);

  printf("\n@END: Sent the GT Inforamtion toc Client: %d\n", ret_val);

  return 0;
}

/*
 ** Description
 **
 ** GT_Client information send to Host sever.
 ** for process the frame to create the ROI for Find D2L. 
 */
int main(int argc , char *argv[])
{
  char *serverIP;
  int port;
  char *csv_file_name;
  int tot_arg = argc;

  if (tot_arg < 3) {
    printf("***Error!! in arguments\n");
    printf("Usage: <server> <serverIP> <portNo> <gtData_filename>\n");
    return -1;
  }

  netrx cliNet; // TODO use malloc to create netrx object.

  serverIP                        = argv[1];
  port                            = atoi(argv[2]);
  csv_file_name                   = argv[3];
  
  cliNet.serverIP                 = serverIP;
  cliNet.port                     = port;
  cliNet.gt_filename              = csv_file_name;
  
  // Network Conenct and sending Receive the IMU DATA from client and send the SL/TS data to client 
  gt_server_embedded(&cliNet);

  return 0;
}