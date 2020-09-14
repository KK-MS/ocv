#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>

#pragma warning(disable:4996)

#include "opencv2/opencv.hpp"

#include "app_struct.h"
#include "packet.h"

using namespace std;
using namespace cv;

// Functions Declation
int cal_D2SL_bearing(app_struct* ptr_struct_obj);

int process_frame_D2SL(app_struct* ptr_struct_obj);

float bearing_angle90deg(float ptr_struct_obj);

int LandmarkOdometry_SL(app_struct* ptr_struct_obj);

int odometry_data_save(app_struct* ptr_struct_obj);

int gps_mounting_offset_SL(app_struct* ptr_struct_obj);

int run_traffic_sign_relocalize_app(app_struct* ptr_server_obj);

int cal_D2TS_bearing(app_struct* ptr_server_obj);

int process_frame_D2TS(app_struct* ptr_server_obj);

int LandmarkOdometry_TS(app_struct* ptr_server_obj);

int gps_mounting_offset_TS(app_struct* ptr_server_obj);

/*
 ** run_traffic_sign_relocalize_app
 **
 ** relocalize the car position using traffic sign detection and D2TS
 */
int run_traffic_sign_relocalize_app(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);

	printf("\nRelocalization using Traffic Sign...");

	Mat frame_TS;

	float GT_D2TS = 0;
	float ODO_D2TS = 0;
	float confidence_TS = 0;	

	// calculate the distance (GT_D2L) betwwen two points IMU_lat_lon & GT_lat_lon > bearing angle
	cal_D2TS_bearing(ptr_struct_obj);

	// For creating the string for load the IMU_Frame in the MAT fucntion to Process the ODO_D2L
	std::stringstream ss_TS;
	ss_TS << "C:/Users/ll_stkapate/Desktop/Messungen_GeneSys/4/Front_Camera_GoPro_Black_Hero_7/GH037534_img/" << ptr_metadata->u4_frame_number_TS << ".jpg";

	// Load the frame to process
	frame_TS = imread(ss_TS.str().c_str(), IMREAD_COLOR);

	printf("\nTS_farme: %s", ss_TS.str().c_str());

	// If the frame is empty, break immediately
	if (frame_TS.empty()) return -1;

	// Pass the loaded IMU_Frame to structure to use it in process_frame() for ODO_D2L
	ptr_struct_obj->frame_TS = frame_TS;

	process_frame_D2TS(ptr_struct_obj);
		

	//find the new car Posiotion Lat_new, Lon_new
	LandmarkOdometry_TS(ptr_struct_obj);

	// Relocalize_GPS (From Front camera Mounting) Correction to the actual GPS_mounting offset
	gps_mounting_offset_TS(ptr_struct_obj);

	// Calculat the confidence of D2TS
	GT_D2TS = ptr_gtMetadata->f4_gt_distance_TS;   // GT_D2TS
	ODO_D2TS = ptr_metadata->f4_odo_distance_TS;   // ODO_D2TS

	confidence_TS = GT_D2TS - ODO_D2TS;

	// Pass the confidence_TS in to stucture
	ptr_metadata->f4_out_odo_confidence_TS = confidence_TS;
		
	return 0;
}

/*
 ** run_side_lane_relocalize_app
 **
 ** relocalize the car position using side lane detection and D2SL
 */
int run_side_lane_relocalize_app(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);

	// Variable initialization from structure
	string IMG_folder_name = ptr_struct_obj->img_folder_name;     // IMU_IMG_Folder path

	// Variables for calulate the cofidence of process
	float GT_D2L = 0;
	float ODO_D2L = 0;
	float confidence = 0;

	// For load the IMU_Frame
	Mat frame_SL;

	// Calculate the distance (GT_D2L) betwwen two points IMU_lat_lon & GT_lat_lon > bearing angle
	cal_D2SL_bearing(ptr_struct_obj);

	printf("\n\nThe Car Relocalization using Side Lane...\n");

	// For creating the string for load the IMU_Frame in the MAT fucntion to Process the ODO_D2L
	std::stringstream ss_imu;
	ss_imu << IMG_folder_name << ptr_metadata->u4_frame_number << ".png";

	// Load the frame to process
	frame_SL = imread(ss_imu.str().c_str(), IMREAD_COLOR);

	// If the frame is empty, break immediately
	if (frame_SL.empty()) return -1;

	// Pass the loaded IMU_Frame to structure to use it in process_frame() for ODO_D2L
	ptr_struct_obj->frame_SL = frame_SL;

	// Process the IMU_Frame fro ODO_D2L to relocalize the car position
	process_frame_D2SL(ptr_struct_obj);

	// Find the new car Posiotion Lat_new, Lon_new
	LandmarkOdometry_SL(ptr_struct_obj);

	// Relocalize_GPS (Front Right Tyre) Correction to the actual GPS_mounting offset
	gps_mounting_offset_SL(ptr_struct_obj);

	// Calculat the Accuracy of ODO_D2L
	GT_D2L  = ptr_gtMetadata->f4_gt_distance;  // GT_D2L
	ODO_D2L = ptr_metadata->f4_odo_distance;   // ODO_D2L

	confidence = GT_D2L - ODO_D2L;

	// Pass the confidence_SL in to stucture
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
int run_car_relocalize_app(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);
	
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
	while (fgets(line_imu, 1024, ptr_struct_obj->imu_file))
	{
		// readed line duplciation to variable for extracting the needed data for procrssing from file
		char* row_cpy_1 = strdup(line_imu);
		char* row_cpy_2 = strdup(line_imu);
		char* row_cpy_3 = strdup(line_imu);
		char* row_cpy_4 = strdup(line_imu);
		char* row_cpy_5 = strdup(line_imu);
		char* row_cpy_6 = strdup(line_imu);
		char* row_cpy_7 = strdup(line_imu);
		char* row_cpy_8 = strdup(line_imu);
		char* row_cpy_9 = strdup(line_imu);

		// get the needed couloumn inforamtion
		string GPS_Time_msec_imu = getfield(row_cpy_1, ptr_struct_obj->time_col);
		string INS_Lat_Abs_imu   = getfield(row_cpy_2, ptr_struct_obj->lat_col);
		string INS_Lon_Abs_imu   = getfield(row_cpy_3, ptr_struct_obj->lon_col);
		string frame_SL_imu      = getfield(row_cpy_4, ptr_struct_obj->frame_SL_no_col);
		string frame_TS_imu      = getfield(row_cpy_5, ptr_struct_obj->frame_TS_no_col);
		string GPS_Mode_imu      = getfield(row_cpy_6, ptr_struct_obj->GPS_col);
		string road_imu          = getfield(row_cpy_7, ptr_struct_obj->road_col);
		string direction_imu     = getfield(row_cpy_8, ptr_struct_obj->direction_col);
		
		// Free the store memory
		free(row_cpy_1);
		free(row_cpy_2);
		free(row_cpy_3);
		free(row_cpy_4);
		free(row_cpy_5);
		free(row_cpy_6);
		free(row_cpy_7);
		free(row_cpy_8);

		// Get the CRO_Data Nearest GT coordinate mapped with given IMU_DATA
		fgets(line_cro, 1024, ptr_struct_obj->cro_file);
		
		// Readed line duplciation to variable for extracting the needed data for procrssing from file
		row_cpy_1 = strdup(line_cro);
		row_cpy_2 = strdup(line_cro);
		row_cpy_3 = strdup(line_cro);
		row_cpy_4 = strdup(line_cro);
		row_cpy_5 = strdup(line_cro);
		row_cpy_6 = strdup(line_cro);
		row_cpy_7 = strdup(line_cro);
		row_cpy_8 = strdup(line_cro);
		row_cpy_9 = strdup(line_cro);

		// Get the needed couloumn inforamtion
		string GT_lat_cro       = getfield(row_cpy_1, ptr_struct_obj->GT_lat_col);
		string GT_lon_cro       = getfield(row_cpy_2, ptr_struct_obj->GT_lon_col);
		string GT_bear_cro      = getfield(row_cpy_3, ptr_struct_obj->GT_bear_col);
		string GT_lat_TS_cro    = getfield(row_cpy_4, ptr_struct_obj->GT_lat_TS_col);
		string GT_lon_TS_cro    = getfield(row_cpy_5, ptr_struct_obj->GT_lon_TS_col);
		string GT_bear_TS_cro   = getfield(row_cpy_6, ptr_struct_obj->GT_bear_TS_col);
		string GT_width_TS_cro  = getfield(row_cpy_7, ptr_struct_obj->GT_width_TS_col);
		string GT_height_TS_cro = getfield(row_cpy_8, ptr_struct_obj->GT_height_TS_col);
		string GT_shape_TS      = getfield(row_cpy_9, ptr_struct_obj->GT_shape_TS_col);
		
		if (!frame_SL_imu.empty() && stoi(frame_SL_imu) != 0 && !GPS_Time_msec_imu.empty() && !INS_Lat_Abs_imu.empty() && !INS_Lon_Abs_imu.empty() && GT_lat_cro != "None" && GT_lon_cro != "None")
		{

			// Convert the string to perticualr data foramt and assing it to stucture variable
			ptr_metadata->u4_timestampL      = stoi(GPS_Time_msec_imu);
			ptr_metadata->d8_ins_latitude    = stod(INS_Lat_Abs_imu);
			ptr_metadata->d8_ins_longitude   = stod(INS_Lon_Abs_imu);
			ptr_metadata->u4_frame_number    = stoi(frame_SL_imu);
			ptr_metadata->u4_frame_number_TS = stoi(frame_TS_imu);
			ptr_metadata->u4_ins_gps_status  = stoi(GPS_Mode_imu);

			// Copying the contents of the string to char array for saving the road information in .csv File
			strcpy(ptr_struct_obj->road, road_imu.c_str());
			strcpy(ptr_struct_obj->direction, direction_imu.c_str());

			printf("\nIMU: %d\t%d\t%0.8f\t%0.8f\t%d\t%s\t%s\n", ptr_metadata->u4_frame_number, ptr_metadata->u4_timestampL
				, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude, ptr_metadata->u4_ins_gps_status
				, ptr_struct_obj->road, ptr_struct_obj->direction);

			// Convert the string to perticualr data foramt and assing it to stucture variable
			ptr_gtMetadata->d8_gt_latitude     = stod(GT_lat_cro);
			ptr_gtMetadata->d8_gt_longitude    = stod(GT_lon_cro);
			ptr_gtMetadata->f4_gt_raodbearing  = stof(GT_bear_cro);

			printf("\nCRO_SL %d: %.8f\t%.8f\n", ptr_metadata->u4_frame_number, ptr_gtMetadata->d8_gt_latitude
				, ptr_gtMetadata->d8_gt_longitude);

			ptr_gtMetadata->d8_gt_latitude_TS = stod(GT_lat_TS_cro);
			ptr_gtMetadata->d8_gt_longitude_TS = stod(GT_lon_TS_cro);
			ptr_gtMetadata->f4_gt_raod_bear2TS = stof(GT_bear_TS_cro);
			ptr_gtMetadata->f4_gt_width_TS = stod(GT_width_TS_cro);
			ptr_gtMetadata->f4_gt_height_TS = stod(GT_height_TS_cro);
			ptr_struct_obj->shape_TS = GT_shape_TS;

			// Copying the contents of the string to char array for saving the road information in .csv File
			strcpy(ptr_struct_obj->TS_shape, GT_shape_TS.c_str());

			printf("\nCRO_TS %d: %s\t%.8f\t%.8f\t%f\t%f\t%f\n", ptr_metadata->u4_frame_number_TS, ptr_struct_obj->TS_shape
				, ptr_gtMetadata->d8_gt_latitude_TS, ptr_gtMetadata->d8_gt_longitude_TS, ptr_gtMetadata->f4_gt_raod_bear2TS
				, ptr_gtMetadata->f4_gt_width_TS, ptr_gtMetadata->f4_gt_height_TS);
					
			// Relocalize the car position using side lane detection and D2SL
			run_side_lane_relocalize_app(ptr_struct_obj);

			// If Traffic info (shape, position or Frame) is available then also Relcoalize the car using D2TS
			if (GT_shape_TS != "None" && ptr_struct_obj->traffic_info == 1)
			{
				// Relocalization using Traffic Sign by D2TS
				run_traffic_sign_relocalize_app(ptr_struct_obj);
			}
			
			// Calculate the Confidence and save the Relocalize data as .csv file
			if ((ptr_struct_obj->VO_data_save_csv == 1)) {
				
				// open the file for saving the process frame data at the end of the loop
				ptr_struct_obj->odometry_file = fopen(ptr_struct_obj->ODO_filename, "a");
			
				odometry_data_save(ptr_struct_obj);

				fclose(ptr_struct_obj->odometry_file);
			}
			
			// Save the process D2L / D2TS frame as a image
			if (ptr_struct_obj->VO_process_frame_save == 1) {

				// To create the filenames as imu_frame number for save the process frames (SL)
				std::stringstream ss;
				ss << "4/Data/img_process_SL/" << ptr_metadata->u4_frame_number << ".png";

				// save process_frame_SL as image
				imwrite(ss.str().c_str(), ptr_struct_obj->frame_SL);

				printf("\nProcess Frame_SL saved as image.");
				// To create the filenames as imu_frame number for save the process frames (TS)
				std::stringstream ss_TS;
				ss_TS << "4/Data/img_process_TS/" << ptr_metadata->u4_frame_number_TS << ".jpg";

				// save process_frame_TS as image
				imwrite(ss_TS.str().c_str(), ptr_struct_obj->frame_TS);

				printf("\nProcess Frame_TS saved as image.");
			}
						
			// Time delay between two frames to prcess in milisecond
			waitKey(0);
		}
		else continue;
		
	} // loop_IMU_data_read
	
	return 0;
}
