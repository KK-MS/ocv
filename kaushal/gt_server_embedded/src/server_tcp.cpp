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

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <string.h>

#include<winsock2.h>

#include "netrx.h"
#include "packet.h"

using namespace std;

//Function Declaration
int receive_request(netrx *ptr_server_obj);

int process_request(netrx *ptr_server_obj);

int execute_request(netrx *ptr_server_obj);

int process_side_lane_info(netrx *ptr_server_obj);

int process_Traffic_sign_info(netrx *ptr_server_obj);

/*
 ** gt_info_send
 **
 ** Send GT Information SideLane / TS to Host client
 */
int receive_req_send_gt_info(netrx *ptr_server_obj)
{
  int read_size;
  int send_size;

  while(1) {

    // Wait for the client request
    read_size = receive_request(ptr_server_obj);
    if(read_size < 0) {
      printf("Error in receive_request: no.:[%d] %s\n", errno, strerror(errno));
      return -1;
    }

    // Process the request
    send_size = process_request(ptr_server_obj);

    if(send_size < 0) {
      printf("Error in process_request: no.:[%d] %s\n", errno, strerror(errno));
      return -1;
    }
  } // loop

  printf("sent GT_METADATA.\n");
  return 0;
}

/*
 ** receive_request
 **
 ** Receiving the Data from client
 */
int receive_request(netrx *ptr_server_obj)
{
  int read_size = recv(ptr_server_obj->sock_desc_gt_bridge, (char *)&(ptr_server_obj->stPacket), sizeof(PACKET), 0);

  if ((read_size < 0)) {
    printf("Error in receive_request: Size: %d, Errno.: [%d] : %s\n", read_size,
        errno, strerror(errno));
    return -1;
  }

  printf("\nS> rx size:%d, rx req:0x%x \n", read_size, ptr_server_obj->stPacket.u4_request_type);
  return read_size;
}

/*
 ** process_request
 **
 ** Process the client Request
 */
int process_request(netrx *ptr_server_obj)
{
  int retVal = 0;
  int req_type =  ptr_server_obj->stPacket.u4_request_type;

  printf("\nS> req: %d \n", req_type);

  switch(req_type) {

    case REQ_GT_LANE_INFO:
      retVal = process_side_lane_info(ptr_server_obj);
      break;

    case REQ_GT_TRAFFIC_SIGN_INFO:
      retVal = process_Traffic_sign_info(ptr_server_obj);
      break;

    default:
      printf("Error in process_request: req_type:[%d]\n", req_type);
      exit(0);
  }

  return retVal;
}

/*
 ** process_side_lane_info
 **
 ** Send the side Lane info to clinet
 ** TODO: Consider using lat, long
 */
int process_side_lane_info(netrx *ptr_server_obj)
{
  // Packet structure define
  GT_LANE_PACKET *ptr_gtMetadata = (GT_LANE_PACKET *) &(ptr_server_obj->stGtLanePacket);
 
  // varibles fro GT_METADA csv file reading for create the ROI
  int row_count = 0;
  string row;
  bool Skip = true;
  char *filename;

  int retVal = 0;

  filename = ptr_server_obj->gt_filename;

  float GT_D2L; // in cm

  // Open the GT_METADA csv file for create the ROI for D2L processing in frame
  ifstream  data(filename);

  // Skip the header line or first line of file
  getline(data, row);

  // read the GT_data line by line and send to client
  while (getline(data, row)) {

    // get the needed GT_METDADA from csv file for process the frame in sequence
    row_count += 1;

    stringstream  rowStream(row);
    string        cell;
    int column_count = 0;

    // extract the words from row in to columns
    while (getline(rowStream, cell, ',')) {

      column_count += 1;

      // You have a cell of GT_D2L
      if (column_count == 5) {

        // check the cell, if empty then replace with 0
        if (cell.empty() && Skip) {

          cell = "0";
          Skip = false;
        }
        GT_D2L = stof(cell);
        cout << endl << cell << " row " << row_count << " column " << column_count << "  Dist: " << GT_D2L;
      }
    }
    ptr_gtMetadata->u4_gt_distance = GT_D2L;

    //Send the Metadata to client
    retVal = execute_request(ptr_server_obj);

  } // loop

  return retVal;
}

/*
 ** process_Traffic_sign_info
 **
 ** Send the Traffic sign info to clinet
 */
int process_Traffic_sign_info(netrx *ptr_server_obj)
{
  // Packet structure define
  GT_LANE_PACKET *ptr_gtMetadata = (GT_LANE_PACKET *) &(ptr_server_obj->stGtLanePacket);

  // varibles fro GT_METADA csv file reading for create the ROI
  int row_count = 0;
  string row;
  bool Skip = true;
  char *filename;

  int retVal = 0;

  filename = ptr_server_obj->gt_filename;

  float GT_D2L; // in cm

  // Open the GT_METADA csv file for create the ROI for D2TS processing in frame
  ifstream  data(filename);

  // Skip the header line or first line of file
  getline(data, row);

  // read the GT_data line by line and send to client
  while (getline(data, row)) {

    // get the needed GT_METDADA from csv file for process the frame in sequence
    row_count += 1;

    stringstream  rowStream(row);
    string        cell;
    int column_count = 0;

    // extract the words from row in to columns
    while (getline(rowStream, cell, ',')) {

      column_count += 1;

      // You have a cell of TS
      if(column_count == 8) {  // For example

        // check the cell, if empty then replace with 0
	if (cell.empty() && Skip) {

     	  cell = "0";
          Skip = false;
        }
        GT_D2L = stof(cell);
        cout << endl << cell << " row " << row_count << " column " << column_count << "  Dist: " << GT_D2L;
      }
    }
    ptr_gtMetadata->u4_gt_distance = GT_D2L;

    //Send the Metadata to client
    retVal = execute_request(ptr_server_obj);
  } // loop

  return retVal;
}

/*
 ** execute_request
 **
 ** sending the Data to client
 */
int execute_request(netrx *ptr_server_obj)
{
  // Packet structure define
  GT_LANE_PACKET *ptr_gtMetadata = (GT_LANE_PACKET *) &(ptr_server_obj->stGtLanePacket);
  
  int send_size;
  
  // Send the GT_data to client
  send_size = send(ptr_server_obj->sock_desc_gt_bridge, (char*)ptr_gtMetadata, sizeof(GT_LANE_PACKET), 0);

  if(send_size < 0) {
    printf("***Error in Sending Data to client: Error no.:[%d] : %s\n"
        , errno, strerror(errno));
    return -1;
  }

  printf("\nsend Size: %d\n", send_size);
  return send_size;
}