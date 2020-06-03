#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>

#pragma warning(disable:4996)

#include "opencv2/opencv.hpp"

#include "netrx1.h"
#include "packet.h"

using namespace std;
using namespace cv;

// Function Declation
int process_frame(netrx* ptr_server_obj);

int LandmarkOdometry(netrx* ptr_server_obj, int bearingAngle_rot);

int bearing_angle90deg(int roadBearing);

int cal_D2L_bearing(netrx* ptr_server_obj, int roadBearing);

int run_app(netrx *ptr_server_obj)
{	
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	int counter = 0;
	int roadBearing_rot;
	bool skip = true;
	int i = 8955;       // save_prcoess_frame

	int FrameCount = 0;
	Mat frame;

	FILE* file;         // file for Update the Relocalize Data

	printf("\nLoad the IMU_file...\n");
	// Open the GT_METADA csv file for create the ROI for D2L processing in frame
	ifstream  imu_data("A7_IMU_24.04.20.csv");
	
	int time;
	int GPS_stauts;
	int frame_imu1;
	string row_imu;
	
	printf("\nLoad the CRO_file...\n");
	// Open the GT_METADA csv file for create the ROI for D2L processing in frame
	ifstream  cro_data("A7_CRO_GT_24.04.20.csv");

	int roadBearing;
	string row_cro;

	// Skip the header line or first line of IMU_file
	getline(imu_data, row_imu);

	// Skip the header line or first line of CRO_file
	getline(cro_data, row_cro);

	// Load the image files from folder in sequence
	vector<cv::String> fn;
	glob("D:/A7_measurement_24.4.20/evening/img/*.png", fn, false); // 24/04/2020

	// For Saving the process frames as video
	const String name = "./A7_24.04.2020_ODO_D2L.avi";

	ptr_server_obj->wrOutVideo.open(name, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, Size(640, 480), true);

	// For add the logo to process_frame display
	const String logo = "./ADrive_Logo1.png";
	Mat mlogo = imread(logo, IMREAD_COLOR);

	// Resize the logo
	cv::resize(mlogo, ptr_server_obj->mat_logo, Size(90, 30));

	// open the file for saving the process frame data at the end of the loop
	file = fopen("A7_IMU_24.04.20_Relocalize.csv", "a");

	fprintf(file,"INS_time_ms, INS_Lat, INS_Lon, Nearest_Lat, Nearest_Lon, GT_D2L_m, Frame_no, ODO_D2L_m, Lat_new, Lon_new, GPS_stauts\n");

	fclose(file);

	// read the GT_data line by line of IMU & CRO DATA to Calculate the D2L
	while (getline(imu_data, row_imu)) {
		
		// get the IMU_Data
		stringstream  rowStream(row_imu);
		string gps_imu;
		string time_imu;
		string lat_imu;
		string lon_imu;
		string frame_imu;
		
		// extract the words from row in to columns in sequence
		getline(rowStream, gps_imu, ',');
		getline(rowStream, time_imu, ',');
		getline(rowStream, lat_imu, ',');
		getline(rowStream, lon_imu, ',');
		getline(rowStream, frame_imu, ',');
		
		// get the CRO_Data Nearest GT coordinate mapped with given IMU_DATA
		getline(cro_data, row_cro);

		if (!frame_imu.empty()) {

			time = stoi(time_imu);
			GPS_stauts = stoi(gps_imu);
			ptr_metadata->d8_ins_latitude  = stod(lat_imu);
			ptr_metadata->d8_ins_longitude = stod(lon_imu);
			ptr_metadata->u4_frame_number  = stoi(frame_imu);

			printf("\n\nFrame %d: %d\t%.8f\t%.8f\n"
				, ptr_metadata->u4_frame_number, time, ptr_metadata->d8_ins_latitude
				, ptr_metadata->d8_ins_longitude);
			
			stringstream  row_stream(row_cro);
			string lat_cro;
			string lon_cro;
			string bearing_cro;

			// extract the words from row in to columns in sequence
			getline(row_stream, lat_cro, ',');
			getline(row_stream, lon_cro, ',');
			getline(row_stream, bearing_cro, ',');

			// convert the data from string to numbers
			ptr_gtMetadata->d8_ins_latitude = stod(lat_cro);
			ptr_gtMetadata->d8_ins_longitude = stod(lon_cro);
			roadBearing = stoi(bearing_cro);
						
			printf("\nCRO %d: %.8f\t%.8f\n", counter, ptr_gtMetadata->d8_ins_latitude
				, ptr_gtMetadata->d8_ins_longitude);
			
			counter++;

			// calculate the distance (GT_D2L) betwwen two points IMU_lat_lon & GT_lat_lon > bearing angle
			cal_D2L_bearing(ptr_server_obj, roadBearing);

			// Load the frame to process
			frame = imread(fn[FrameCount].c_str(), IMREAD_COLOR);

			// If the frame is empty, break immediately
			if (frame.empty()) break;

			ptr_server_obj->frame = frame;
			ptr_server_obj->frame_name = fn[FrameCount].c_str();

			// to create the filenames in sequence for save the process frames
			std::stringstream ss;
			ss << "D:/A7_measurement_24.4.20/evening/img_process/" << i << ".png";
			
			// process the IMU_Frame fro ODO_D2L to relocalize the car position
			process_frame(ptr_server_obj);

			// Bearing angle rotarte to lateral Axis
			roadBearing_rot = bearing_angle90deg(roadBearing);

			//find the new car Posiotion Lat_new, Lon_new
			LandmarkOdometry(ptr_server_obj, roadBearing_rot);

			// open the file for saving the process frame data at the end of the loop
			file = fopen("A7_IMU_24.04.20_Relocalize.csv", "a");

			fprintf(file,"%d, %.8f, %.8f, %.8f, %.8f, %f, %d, %f, %.8f, %.8f, %d\n"
				, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
				, ptr_gtMetadata->d8_ins_latitude, ptr_gtMetadata->d8_ins_longitude
				, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
				, ptr_metadata->f4_odo_distance, ptr_gtMetadata->d8_gt_latitude
				, ptr_gtMetadata->d8_gt_longitude, GPS_stauts);
			
			fclose(file);

			// save process_frame as image
			imwrite(ss.str().c_str(), frame);

			waitKey(50);
		}
		else continue;

		i++;
		FrameCount++;

	} // loop_IMU_data_read
	
	return 0;
}

int main(int argc, char* argv[])
{
	// structure define
	netrx serNet;

	// Load the IMU & CRO Data files & process the GT_D2L & ODO_D2L > Relocalization of car position 
	run_app(&serNet);

	return 0;
}
