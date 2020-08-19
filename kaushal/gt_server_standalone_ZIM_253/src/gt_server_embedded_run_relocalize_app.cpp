#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>

#pragma warning(disable:4996)

#include "opencv2/opencv.hpp"

#include "netrx.h"
#include "packet.h"

using namespace std;
using namespace cv;

// Functions Declation
int cal_D2SL_bearing(netrx* ptr_server_obj);

int process_frame_D2SL(netrx* ptr_server_obj);

float bearing_angle90deg(float roadBearing);

int LandmarkOdometry(netrx* ptr_server_obj);

int odometry_data_save(netrx* ptr_server_obj);

int gps_mounting_offset(netrx* ptr_server_obj);

/*
 ** run_traffic_sign_relocalize_app
 **
 ** relocalize the car position using traffic sign detection and D2TS
 */
int run_traffic_sign_relocalize_app(netrx* ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	printf("\nThe Relocalization using traffic sign is under progress...");

	return 0;
}

/*
 ** run_side_lane_relocalize_app
 **
 ** relocalize the car position using side lane detection and D2SL
 */
int run_side_lane_relocalize_app(netrx* ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	// Variable initialization from structure
	string IMG_folder_name = ptr_server_obj->img_folder_name;     // IMU_IMG_Folder path

	// Variables for calulate the cofidence of process
	float GT_D2L = 0;
	float ODO_D2L = 0;
	float confidence = 0;

	// For load the IMU_Frame
	Mat frame;

	// Calculate the distance (GT_D2L) betwwen two points IMU_lat_lon & GT_lat_lon > bearing angle
	cal_D2SL_bearing(ptr_server_obj);

	printf("\n\nThe Car Relocalization using Side Lane...\n");

	// For creating the string for load the IMU_Frame in the MAT fucntion to Process the ODO_D2L
	std::stringstream ss_imu;
	ss_imu << IMG_folder_name << ptr_metadata->u4_frame_number << ".png";

	// Load the frame to process
	frame = imread(ss_imu.str().c_str(), IMREAD_COLOR);

	// If the frame is empty, break immediately
	if (frame.empty()) return -1;

	// Pass the loaded IMU_Frame to structure to use it in process_frame() for ODO_D2L
	ptr_server_obj->frame = frame;

	// Process the IMU_Frame fro ODO_D2L to relocalize the car position
	process_frame_D2SL(ptr_server_obj);

	// Find the new car Posiotion Lat_new, Lon_new
	LandmarkOdometry(ptr_server_obj);

	// Relocalize_GPS (Front Right Tyre) Correction to the actual GPS_mounting offset
	gps_mounting_offset(ptr_server_obj);

	// Calculat the Accuracy of ODO_D2L
	GT_D2L = ptr_gtMetadata->f4_gt_distance;  // GT_D2L
	ODO_D2L = ptr_metadata->f4_odo_distance;   // ODO_D2L

	confidence = GT_D2L - ODO_D2L;

	ptr_metadata->f4_out_odo_confidence = confidence;
	
	return 0;
}

/*
 ** getfield
 **
 ** Extract the selected Coloumn information from the csv file
 */
const char* getfield(char* line, int col_num)
{
	const char* col_data;

	for (col_data = strtok(line, ","); col_data&& *col_data; col_data = strtok(NULL, ",\n"))
	{
		if (!-- col_num)
			return col_data;
	}

	return NULL;
}

/*
 ** run_side_lane_relocalize_app
 **
 ** Load the IMU & CRO Data files & process the GT_D2L & ODO_D2L
 ** Relocalization of car position using Side lane
 */
int run_car_relocalize_app(netrx* ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);
	
	// Variables for calulate the cofidence of process
	float GT_D2L = 0;
	float ODO_D2L = 0;
	float confidence = 0;

	// For load the IMU_Frame
	Mat frame;

	printf("\nReading the IMU & CRO files Data..\n");
	
	// For reading the row_data from csv file
	char line_imu[1024];
	
	// For reading the row_data from csv file
	char line_cro[1024];

	// Read the Selected coloumns data for further processing of relocalize the car Position
	while (fgets(line_imu, 1024, ptr_server_obj->imu_file))
	{
		// readed line duplciation to variable for extracting the needed data for procrssing from file
		char* row_cpy_1 = strdup(line_imu);
		char* row_cpy_2 = strdup(line_imu);
		char* row_cpy_3 = strdup(line_imu);
		char* row_cpy_4 = strdup(line_imu);
		char* row_cpy_5 = strdup(line_imu);
		char* row_cpy_6 = strdup(line_imu);
		char* row_cpy_7 = strdup(line_imu);
		
		// get the needed couloumn inforamtion
		string GPS_Time_msec_imu = getfield(row_cpy_1, ptr_server_obj->time_col);
		string INS_Lat_Abs_imu   = getfield(row_cpy_2, ptr_server_obj->lat_col);
		string INS_Lon_Abs_imu   = getfield(row_cpy_3, ptr_server_obj->lon_col);
		string frame_imu         = getfield(row_cpy_4, ptr_server_obj->frame_no_col);
		string GPS_Mode_imu      = getfield(row_cpy_5, ptr_server_obj->GPS_col);
		string road_imu          = getfield(row_cpy_6, ptr_server_obj->road_col);
		string direction_imu     = getfield(row_cpy_7, ptr_server_obj->direction_col);
		
		// Convert the string to perticualr data foramt and assing it to stucture variable
		ptr_metadata->u4_timestampL     = stoi(GPS_Time_msec_imu);
		ptr_metadata->d8_ins_latitude   = stod(INS_Lat_Abs_imu);
		ptr_metadata->d8_ins_longitude  = stod(INS_Lon_Abs_imu);
		ptr_metadata->u4_frame_number   = stoi(frame_imu);
		ptr_metadata->u4_ins_gps_status = stoi(GPS_Mode_imu);

		// Copying the contents of the string to char array for saving the road information in .csv File
		strcpy(ptr_server_obj->road, road_imu.c_str());
		strcpy(ptr_server_obj->direction, direction_imu.c_str());
		
		// Free the store memory
		free(row_cpy_1);
		free(row_cpy_2);
		free(row_cpy_3);
		free(row_cpy_4);
		free(row_cpy_5);
		free(row_cpy_6);
		free(row_cpy_7);
		
		// Get the CRO_Data Nearest GT coordinate mapped with given IMU_DATA
		fgets(line_cro, 1024, ptr_server_obj->cro_file);

		// Readed line duplciation to variable for extracting the needed data for procrssing from file
		row_cpy_1 = strdup(line_cro);
		row_cpy_2 = strdup(line_cro);
		row_cpy_3 = strdup(line_cro);

		// Get the needed couloumn inforamtion
		string GT_lat_cro = getfield(row_cpy_1, ptr_server_obj->GT_lat_col);
		string GT_lon_cro = getfield(row_cpy_2, ptr_server_obj->GT_lon_col);
		string GT_bear_cro = getfield(row_cpy_3, ptr_server_obj->GT_bear_col);
		
		if (!frame_imu.empty() && stoi(frame_imu) != 0 && !GPS_Time_msec_imu.empty() && !INS_Lat_Abs_imu.empty() && !INS_Lon_Abs_imu.empty() && GT_lat_cro != "None" && GT_lon_cro != "None")
		{
			// Convert the string to perticualr data foramt and assing it to stucture variable
			ptr_gtMetadata->d8_gt_latitude    = stod(GT_lat_cro);
			ptr_gtMetadata->d8_gt_longitude   = stod(GT_lon_cro);
			ptr_gtMetadata->f4_gt_raodbearing = stof(GT_bear_cro);
			
			// Free the store memory
			free(row_cpy_1);
			free(row_cpy_2);
			free(row_cpy_3);
	
			printf("\nIMU: %d\t%d\t%0.8f\t%0.8f\t%d\t%s\t%s\n", ptr_metadata->u4_frame_number, ptr_metadata->u4_timestampL
				, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude, ptr_metadata->u4_ins_gps_status
				, ptr_server_obj->road, ptr_server_obj->direction);

			printf("\nCRO %d: %.8f\t%.8f\n", ptr_metadata->u4_frame_number, ptr_gtMetadata->d8_gt_latitude
				, ptr_gtMetadata->d8_gt_longitude);

			// Relocalize the car position using side lane detection and D2SL
			run_side_lane_relocalize_app(ptr_server_obj);

			// If Traffic info (shape, position or Frame) is available then also Relcoalize the car using D2TS
			if (ptr_server_obj->traffic_info == 1)
			{
				run_traffic_sign_relocalize_app(ptr_server_obj);
			}
			
			// Calculate the Confidence and save the Relocalize data as .csv file
			if ((ptr_server_obj->VO_data_save_csv == 1)) {
				
				// open the file for saving the process frame data at the end of the loop
				ptr_server_obj->odometry_file = fopen(ptr_server_obj->ODO_filename, "a");
			
				odometry_data_save(ptr_server_obj);

				fclose(ptr_server_obj->odometry_file);
			}
			
			// Save the process D2L frame as a image
			if (ptr_server_obj->VO_process_frame_save == 1) {

				// To create the filenames as imu_frame number for save the process frames
				std::stringstream ss;
				ss << "Data/img_process/" << ptr_metadata->u4_frame_number << ".png";

				// save process_frame as imag
				imwrite(ss.str().c_str(), frame);
			}

			// Time delay between two frames to prcess in milisecond
			waitKey(1);
		}
		else continue;
		
	} // loop_IMU_data_read
	
	return 0;
}