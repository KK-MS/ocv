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

// Frame Status
#define FRAME_STATUS_8  8
#define FRAME_STATUS_4  4
#define FRAME_STATUS_2  2
#define FRAME_STATUS_1  1
#define FRAME_STATUS_0  0

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
  
  // File names for Input data
  string cro_filename;	    // CRO_csv filename
  string imu_filename;	    // IMU_csv filename
  string img_folder_name;   // IMU_IMG_Folder path
  string odometry_filename; // Odometry_csv filename

  // Input Data for csv file reading
  string GPS_Time;
  string INS_Lat;
  string INS_Lon;
  string frame_number;
  string GPS_status;
  string Road;
  string Direction;
  string GT_lat;
  string GT_lon;
  string GT_road_bearing;

  // Process / Visual Odometry data saving options > Get the info from config.json file
  int VO_data_save_csv;
  int VO_process_frame_save;
  int VO_process_frames_video_save;
  int traffic_info;
  
  int start_X;
  int start_Y;
  int end_Y;  
  int frame_width;
  float pixel_dist;
  float cali_max_D2L;
  
  char ODO_filename[50];
    
  // File name for Odometry Data save
  FILE *odometry_file;
  
  // FILE for load the IMU and CRO files
  FILE* imu_file;
  FILE* cro_file;
  
  // variable to assign the coloumn number according to the needed header information
  int time_col = 0;
  int lat_col = 0;
  int lon_col = 0;
  int frame_no_col = 0;
  int GPS_col = 0;
  int road_col = 0;
  int direction_col = 0;
  
  // variable to assign the coloumn number according to the needed header information
  int GT_lat_col = 0;
  int GT_lon_col = 0;
  int GT_bear_col = 0;
  
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