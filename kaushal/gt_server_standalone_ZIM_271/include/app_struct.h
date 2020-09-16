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

#ifndef APP_STRUCT
#define APP_STRUCT

#include <fstream>

#include "packet.h"

#include "opencv2/opencv.hpp"

#define R	 6378137.0               // Radius from the center of the earth to the equator
#define EN	 0.0818191908426         // Eccentricity
#define M_PI 3.14159265358979323846

// MACROS

// Frame Status
#define FRAME_STATUS_8  8
#define FRAME_STATUS_4  4
#define FRAME_STATUS_2  2
#define FRAME_STATUS_1  1
#define FRAME_STATUS_0  0

// for the creating the ROI to process the D2SL based on IMU GPS_Status
#define GPS_STATUS_8    8
#define GPS_STATUS_4    4
#define GPS_STATUS_2    2
#define GPS_STATUS_1    1

#define RUN_RELOCALISATION_APP       1
#define RUN_VIDEO_PROCESS_D2SL_APP   1
  
#define VO_DATA_SAVE                 1
#define VO_PROCESS_FRAME_SAVE        1 
#define VO_PROCESS_FRAME_VIDEO_SAVE  1
#define TRAFFIC_INFO                 1
#define VIDEO_PROCESS_D2SL           1
#define RELOCALISTION_APP_RUN        1
#define ADRIVE_LOGO                  1




// Namespace
using namespace std;
using namespace cv;

// STRUCTURES

// APP_STRUCT: Application main structure
typedef struct app_struct {

  // Request type
  PACKET stPacket;

  // GT Lane packet
  GT_LANE_PACKET stGtLanePacket;
  
  // File names for Input data
  string cro_filename;	    // CRO_csv filename
  string imu_filename;	    // IMU_csv filename
  string img_folder_name;   // IMU_IMG_Folder path
  string odometry_filename; // Odometry_csv filename
  
  string Process_video_filename; //to be process video for D2SL

  string shape_TS;          // shape_TS
  
  // Input Data for csv file reading
  string gps_time;
  string ins_lat;
  string ins_lon;
  string frame_number_SL;
  string frame_number_TS;
  string gps_status;
  string road;
  string direction;
  string gt_lat;
  string gt_lon;
  string gt_road_bearing;
  string gt_lat_TS;
  string gt_lon_TS;
  string gt_road_bearing_TS;
  string gt_width_TS;
  string gt_height_TS;
  string gt_shape_TS;

  // Process / Visual Odometry data saving options > Get the info from config.json file
  int vo_data_save_csv;
  int vo_process_frame_save;
  int vo_process_frames_video_save;
  int side_lane_info;
  int traffic_info;
  int process_video_D2SL;
  int relocalisation_app;
  int adrive_logo;
  int frame_process_delay_ms;    // Between two frames delay in milisecond
  
  // variables for process the Visual Odometry from congig.txt file
  int start_X;
  int start_Y;
  int end_Y;  
  int frame_width;
  float pixel_dist;
  float cali_max_D2L;
    
  // FILE for load the IMU, CRO & Odometry files
  FILE *imu_file;
  FILE *cro_file;
  FILE *odometry_file;
  
  // variable to assign the IMU file coloumn number according to the needed header information
  int time_col = 0;
  int lat_col = 0;
  int lon_col = 0;
  int frame_SL_no_col = 0;
  int frame_TS_no_col = 0;
  int GPS_col = 0;
  int road_col = 0;
  int direction_col = 0;
    
  // variable to assign the CRO file coloumn number according to the needed header information
  int gt_lat_col = 0;
  int gt_lon_col = 0;
  int gt_bear_col = 0;
  int gt_lat_TS_col = 0;
  int gt_lon_TS_col = 0;
  int gt_bear_TS_col = 0;
  int gt_width_TS_col = 0;
  int gt_height_TS_col = 0;
  int gt_shape_TS_col = 0;
  
  // variables for printing the road info on the Process Frame / Odometry.csv file
  char odo_filename[50];
  char road_name[15];
  char direction_name[15];
  char TS_shape[15];
  
  char video_filename[100];
  
  // variables for GPS mounting Position (Offset) from FR wheel
  float gps_lat_offset;
  float gps_lon_offset;

  // ROI parameters for process the direct Video to D2L without GT Info
  int roi_x1;
  int roi_y1;
  int roi_x2;
  int roi_y2;
  
  // Variables for load the IMU frame_SL_TS / Logo
  Mat frame_SL;
  Mat frame_TS;
  Mat mat_logo_SL;
  Mat mat_logo_TS;
  
  // Process Frames save as Video
  VideoWriter wrOutVideo;

} app_struct;

#endif /* APP_STRUCT */