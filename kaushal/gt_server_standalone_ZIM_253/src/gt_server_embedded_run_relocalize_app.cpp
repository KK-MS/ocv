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
int cal_D2L_bearing(netrx* ptr_server_obj, float roadBearing);

int process_frame(netrx* ptr_server_obj);

float bearing_angle90deg(float roadBearing);

int LandmarkOdometry(netrx* ptr_server_obj, int bearingAngle_rot);

int odometry_data_save(netrx* ptr_server_obj);

int gps_mounting_offset(netrx* ptr_server_obj);

int run_traffic_sign_relocalize_app(netrx* ptr_server_obj, float roadbearing);

/*
 ** getfield
 **
 ** extract the perticular coloumn information from the csv file
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

	// Variable initialization from structure
	string CRO_file          = ptr_server_obj->cro_filename;        // CRO_csv filename
	string IMU_file          = ptr_server_obj->imu_filename;        // IMU_csv filename
	string IMG_folder_name   = ptr_server_obj->img_folder_name;     // IMU_IMG_Folder path
	string Odometry_filename = ptr_server_obj->odometry_filename;   // Odometry_csv filename
	
	char cro_filename[100];
	char imu_filename[100];

	// Copying the contents of the string to char array to laod the .csv Files
	strcpy(cro_filename, IMU_file.c_str());
	strcpy(imu_filename, CRO_file.c_str());

	// Copy the string to the char *variable for FILE reading function (fopen)
	strcpy(ptr_server_obj->ODO_filename, Odometry_filename.c_str());

	// Variables for calulate the cofidence of process  
	float GT_D2L = 0;
	float ODO_D2L = 0;
	float confidence = 0;

	// For load the IMU_Frame
	Mat frame;

	// File for Update the Relocalize Data
	FILE* ODO_file;
	
	// For store the readed line data from csv file (IMU & CRO)
	string row_imu;
	string row_cro;
	
	// Values from csv files > string to specific variables
	//int frame_imu1;
	float roadBearing;
			
	// open the file and write the header line for saving the process Visual Odometry data
	if (RELOCALIZE_DATA_SAVE == 1) {

		ODO_file = fopen(ptr_server_obj->ODO_filename, "a");
  	   
		fprintf(ODO_file, "INS_time_ms,INS_Lat,INS_Lon,Nearest_Lat,Nearest_Lon,GT_D2L_m,Frame_no,ODO_D2L_m,Lat_new,Lon_new,GPS_stauts,Frame_status,Confidence,INS_lat_antenna,INS_lon_antenna\n");

	    fclose(ODO_file);
    }	

	printf("\nLoad the IMU_file...\n");

	FILE* imu_data = fopen(imu_filename, "r");
	
	// Coloumn counting counter
	int col_num_imu = 1;

	// variable to assign the coloumn number according to the needed header information
	int time = 0;
	int lat = 0;
	int lon = 0;
	int frame_no = 0; 
	int GPS = 0;
	int road = 0;
	int direction = 0;
	
	// For reading the row_data from csv file
	char line_imu[1024];

	// Read the Header line from the csv file to select the require columns by its header name
	fgets(line_imu, 1024, imu_data);
	
	// Store the coloumn inforamtion from the readed row_data 
	const char* col_imu;

	// Based on coloumn Header string information, Give the number coloumn for extracting the perticular coloumn data
	for (col_imu = strtok(line_imu, ","); col_imu && *col_imu; col_imu = strtok(NULL, ",\n"))
	{
		printf("\n%s", col_imu);

		if (strcmp(col_imu, "GPS_Time_msec") == 0)
		{
			time = col_num_imu;
			printf("\ttime = %d", time);
		
		} else if (strcmp(col_imu, "INS_Lat_Abs") == 0) {
			lat = col_num_imu;
			printf("\tlat = %d", lat);
		}
		else if (strcmp(col_imu, "INS_Long_Abs") == 0) {

			lon = col_num_imu;
			printf("\tlon = %d", lon);
		}
		else if (strcmp(col_imu, "FrameNumber") == 0) {

			frame_no = col_num_imu;
			printf("\tframe = %d", frame_no);
		}
		else if (strcmp(col_imu, "Status_GPS_Mode") == 0) {
			GPS = col_num_imu;
			printf("\tGPS = %d", GPS);
		}
		else if (strcmp(col_imu, "road") == 0) {

			road = col_num_imu;
			printf("\troad = %d", road);
		}
		else if (strcmp(col_imu, "direction") == 0) {

			direction = col_num_imu;
			printf("\tdirection = %d", direction);
		}

		col_num_imu ++;
	}
	
	printf("\n\nLoad the CRO_file...\n");

	FILE* cro_data = fopen(cro_filename, "r");

	// For reading the row_data from csv file
	char line_cro[1024];

	// Read the Header line from the csv file to select the require columns by its header name
	fgets(line_cro, 1024, cro_data);

	// Coloumn counting counter
	int col_num_cro = 1;

	// variable to assign the coloumn number according to the needed header information
	int GT_lat = 0;
	int GT_lon = 0;
	int GT_bear = 0;
	
	// Store the coloumn inforamtion from the readed row_data
	const char* tok_cro;

	// Based on coloumn Header string information, Give the number coloumn for extracting the perticular coloumn data
	for (tok_cro = strtok(line_cro, ","); tok_cro && *tok_cro; tok_cro = strtok(NULL, ",\n"))
	{
		printf("\n%s", tok_cro);

		if (strcmp(tok_cro, "GT_lat") == 0)
		{
			GT_lat = col_num_cro;
			printf("\tGT_lat = %d", GT_lat);

		}
		else if (strcmp(tok_cro, "GT_lon") == 0) {

			GT_lon = col_num_cro;
			printf("\tGT_lon = %d", GT_lon);
		}
		else if (strcmp(tok_cro, "GT_bear") == 0) {

			GT_bear = col_num_cro;
			printf("\tGT_bear = %d", GT_bear);
		}
				
		col_num_cro ++;
	}

	// Read the Selected coloumns data for further processing of relocalize the car Position
	while (fgets(line_imu, 1024, imu_data))
	{
		// readed line duplciation to variable for extracting the needed data for procrssing from file
		char* imu1 = strdup(line_imu);
		char* imu2 = strdup(line_imu);
		char* imu3 = strdup(line_imu);
		char* imu4 = strdup(line_imu);
		char* imu5 = strdup(line_imu);
		char* imu6 = strdup(line_imu);
		char* imu7 = strdup(line_imu);
		
		// get the needed couloumn inforamtion
		string GPS_Time_msec_imu = getfield(imu1, time);
		string INS_Lat_Abs_imu   = getfield(imu2, lat);
		string INS_Lon_Abs_imu   = getfield(imu3, lon);
		string frame_imu         = getfield(imu4, frame_no);
		string GPS_Mode_imu      = getfield(imu5, GPS);
		string road_imu          = getfield(imu6, road);
		string direction_imu     = getfield(imu7, direction);
		
		// Convert the string to perticualr data foramt and assing it to stucture variable
		ptr_metadata->u4_timestampL     = stoi(GPS_Time_msec_imu);
		ptr_metadata->d8_ins_latitude   = stod(INS_Lat_Abs_imu);
		ptr_metadata->d8_ins_longitude  = stod(INS_Lon_Abs_imu);
		ptr_metadata->u4_frame_number   = stoi(frame_imu);
		ptr_metadata->u4_ins_gps_status = stoi(GPS_Mode_imu);

		// Copying the contents of the string to char array for saving the road information in .csv File
		strcpy(ptr_server_obj->road, road_imu.c_str());
		strcpy(ptr_server_obj->direction, direction_imu.c_str());
		
		printf("\nIMU: %d\t%d\t%0.8f\t%0.8f\t%d\t%s\t%s\n", ptr_metadata->u4_frame_number, ptr_metadata->u4_timestampL
			, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude, ptr_metadata->u4_ins_gps_status
			, ptr_server_obj->road, ptr_server_obj->direction);

		// Free the store memory
		free(imu1);
		free(imu2);
		free(imu3);
		free(imu4);
		free(imu5);
		free(imu6);
		free(imu7);
				
		// Get the CRO_Data Nearest GT coordinate mapped with given IMU_DATA
		fgets(line_cro, 1024, cro_data);

		// Readed line duplciation to variable for extracting the needed data for procrssing from file
		char* cro1 = strdup(line_cro);
		char* cro2 = strdup(line_cro);
		char* cro3 = strdup(line_cro);

		// Get the needed couloumn inforamtion
		string GT_lat_cro = getfield(cro1, GT_lat);
		string GT_lon_cro = getfield(cro2, GT_lon);
		string GT_bear_cro = getfield(cro3, GT_bear);
		
		if (!frame_imu.empty() && stoi(frame_imu) != 0 && !GPS_Time_msec_imu.empty() && !INS_Lat_Abs_imu.empty() && !INS_Lon_Abs_imu.empty() && GT_lat_cro != "None" && GT_lon_cro != "None")
		{			
			// Convert the string to perticualr data foramt and assing it to stucture variable
			ptr_gtMetadata->d8_gt_latitude  = stod(GT_lat_cro);				
			ptr_gtMetadata->d8_gt_longitude = stod(GT_lon_cro);			
			roadBearing                     = stof(GT_bear_cro);
			
			// Free the store memory
			free(cro1);
			free(cro2);
			free(cro3);
	
			printf("\nCRO %d: %.8f\t%.8f\n", ptr_metadata->u4_frame_number, ptr_gtMetadata->d8_gt_latitude
				, ptr_gtMetadata->d8_gt_longitude);

			// Calculate the distance (GT_D2L) betwwen two points IMU_lat_lon & GT_lat_lon > bearing angle
			cal_D2L_bearing(ptr_server_obj, roadBearing);
				
			printf("\n\nThe Car Relocalization using Side Lane...\n");

			// For creating the string for load the IMU_Frame in the MAT fucntion to Process the ODO_D2L
			std::stringstream ss_imu;
			ss_imu << IMG_folder_name << ptr_metadata->u4_frame_number << ".png";
			
			// Load the frame to process
			frame = imread(ss_imu.str().c_str(), IMREAD_COLOR);

			// If the frame is empty, break immediately
			if (frame.empty()) break;

			// Pass the loaded IMU_Frame to structure to use it in process_frame() for ODO_D2L
			ptr_server_obj->frame = frame;
			
			// Process the IMU_Frame fro ODO_D2L to relocalize the car position
			process_frame(ptr_server_obj);
				
			// Bearing angle rotarte to lateral Axis to calculate the new Lat & Lon for car Re-localization
			int roadBearing_rot = bearing_angle90deg(roadBearing);
			
			// Find the new car Posiotion Lat_new, Lon_new
			LandmarkOdometry(ptr_server_obj, roadBearing_rot);
				
			// Relocalize_GPS (Front Right Tyre) Correction to the actual GPS_mounting offset
			gps_mounting_offset(ptr_server_obj);
				
			// If Traffic info or Frame is available then also Relcoalize the car using D2TS
			if (TRAFFIC_INFO == 1)
			{
				run_traffic_sign_relocalize_app(ptr_server_obj, roadBearing);
			}
				
			// Calculat the confidence
			GT_D2L  = ptr_gtMetadata->f4_gt_distance;  // GT_D2L
			ODO_D2L = ptr_metadata->f4_odo_distance;   // ODO_D2L
			
			confidence = GT_D2L - ODO_D2L;

			ptr_metadata->f4_out_odo_confidence = confidence;
	
			// Calculate the Confidence and save the Relocalize data as .csv file
			if ((RELOCALIZE_DATA_SAVE == 1)) {
				
				// open the file for saving the process frame data at the end of the loop
				ODO_file = fopen(ptr_server_obj->ODO_filename, "a");
			
				ptr_server_obj->odometry_file = ODO_file;

				odometry_data_save(ptr_server_obj);

				fclose(ODO_file);
			}
			
			// Save the process D2L frame as a image
			if (PROCESS_FRAME_SAVE == 1) {

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

/*
 ** run_traffic_sign_relocalize_app
 **
 ** relocalize the car position using traffic sign detection and D2TS
 */
int run_traffic_sign_relocalize_app(netrx* ptr_server_obj, float roadbearing)
{
	printf("\nThe Relocalization using traffic sign is under progress...");
	
	return 0;
}