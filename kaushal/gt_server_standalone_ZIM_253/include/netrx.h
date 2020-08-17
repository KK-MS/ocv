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

#include <fstream>

#include "packet.h"

#include "opencv2/opencv.hpp"

#define R	 6378137.0               // Radius from the center of the earth to the equator
#define EN	 0.0818191908426         // Eccentricity
#define M_PI 3.14159265358979323846

// Saving process data
#define RELOCALIZE_DATA_SAVE       1
#define PROCESS_FRAME_SAVE         0
#define PROCESS_FRAMES_VIDEO_SAVE  0

// Frame Status
#define FRAME_STATUS_8  8
#define FRAME_STATUS_4  4
#define FRAME_STATUS_2  2
#define FRAME_STATUS_1  1
#define FRAME_STATUS_0  0

#define TRAFFIC_INFO 0

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
  
  // ADMA packet
  ADMA_PACKET stAdmaPacket;
  
  // File names for Input data
  string cro_filename;	    // CRO_csv filename
  string imu_filename;	    // IMU_csv filename
  string img_folder_name;   // IMU_IMG_Folder path
  string odometry_filename; // Odometry_csv filename
  
  char ODO_filename[50];
  
  // File name for Odometry Data save
  FILE *odometry_file;
  
  // variables for printing the road info on the Process Frame
  char road[15];
  char direction[15];

  // variables for GPS mounting Position (Offset) from FR wheel
  float gps_lat_offset;
  float gps_lon_offset;

  // Process_frame Variables
  Mat frame;
  Mat mat_logo;
  
  const char *frame_name;
  const char *frame_save_no;
  
  // Process Frames save as Video
  VideoWriter wrOutVideo;

} netrx;

#endif /* NETRX_H */