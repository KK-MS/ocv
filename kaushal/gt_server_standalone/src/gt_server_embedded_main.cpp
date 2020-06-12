#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>

#pragma warning(disable:4996)

#include "opencv2/opencv.hpp"

#include "netrx.h"
#include "packet.h"

#define RELOCALIZE_DATA_SAVE 1

using namespace std;
using namespace cv;

// Functions Declation
int process_frame(netrx* ptr_server_obj);

int LandmarkOdometry(netrx* ptr_server_obj, int bearingAngle_rot);

int bearing_angle90deg(int roadBearing);

int cal_D2L_bearing(netrx* ptr_server_obj, int roadBearing);

/*
 ** run_app
 **
 ** Load the IMU & CRO Data files & process the GT_D2L & ODO_D2L 
 ** Relocalization of car position
 */
int run_app(netrx* ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	// Variable initialization from structure
	char* CRO_filename     = ptr_server_obj->cro_filename;       // CRO_csv filename
	char* IMU_filename     = ptr_server_obj->imu_filename;       // IMU_csv filename
	char* IMG_folder_name  = ptr_server_obj->img_folder_name;    // IMU_IMG_Folder path
	char* ODO_filename     = ptr_server_obj->odometry_filename;  // Odometry_csv filename
	
	int counter = 0;
	int roadBearing_rot;
	
	int FrameCount = 0;
	Mat frame;

	FILE* file;        // file for Update the Relocalize Data

	printf("\nLoad the IMU_file...\n");
	// Open the GT_METADA csv file for create the ROI for D2L processing in frame
	ifstream  imu_data(IMU_filename);
	
	int time;
	int GPS_stauts;
	int frame_imu1;
	string row_imu;

	printf("\nLoad the CRO_file...\n");
	// Open the GT_METADA csv file for create the ROI for D2L processing in frame
	ifstream  cro_data(CRO_filename);

	int roadBearing;
	string row_cro;

	// Skip the header line or first line of IMU_file
	//getline(imu_data, row_imu);
	getline(imu_data, row_imu);

	// Skip the header line or first line of CRO_file
	getline(cro_data, row_cro);

	// Load the image files from folder in sequence
	vector<cv::String> fn;
	glob(IMG_folder_name, fn, false); // 24/04/2020
	
	// open the file and write the header line for saving the process frame data
	if (RELOCALIZE_DATA_SAVE == 1) {

		file = fopen(ODO_filename, "a");
  	    fprintf(file, "INS_time_ms, INS_Lat, INS_Lon, Nearest_Lat, Nearest_Lon, GT_D2L_m, Frame_no, ODO_D2L_m, Lat_new, Lon_new, GPS_stauts\n");

	    fclose(file);
    }

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

			// To create the filenames as imu_frame number for save the process frames
			std::stringstream ss;
			ss << "D:/A7_measurement_24.4.20/evening/img_process1/" << ptr_metadata->u4_frame_number << ".png";
			
			// process the IMU_Frame fro ODO_D2L to relocalize the car position
			process_frame(ptr_server_obj);

			// Bearing angle rotarte to lateral Axis to calculate the new Lat & Lon for car Re-localization
			roadBearing_rot = bearing_angle90deg(roadBearing);

			//find the new car Posiotion Lat_new, Lon_new
			LandmarkOdometry(ptr_server_obj, roadBearing_rot);

			if (RELOCALIZE_DATA_SAVE == 1) {
				
				// open the file for saving the process frame data at the end of the loop
			    file = fopen(ODO_filename, "a");

			    fprintf(file,"%d, %.8f, %.8f, %.8f, %.8f, %f, %d, %f, %.8f, %.8f, %d\n"
			    	, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			    	, ptr_gtMetadata->d8_ins_latitude, ptr_gtMetadata->d8_ins_longitude
			    	, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			    	, ptr_metadata->f4_odo_distance, ptr_gtMetadata->d8_gt_latitude
			    	, ptr_gtMetadata->d8_gt_longitude, GPS_stauts);

				fclose(file);
			}

			// save process_frame as image
			//imwrite(ss.str().c_str(), frame);

			waitKey(50);
		}
		else continue;

		FrameCount++;

	} // loop_IMU_data_read
	
	return 0;
}

/*
 ** Init
 **
 ** Data initialization
 */
int Init(netrx *ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);
	
	// For Saving the process frames as video
	const String name = "./A7_24.04.2020_ODO_D2L.avi";

	ptr_server_obj->wrOutVideo.open(name, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, Size(640, 480), true);

	// For add the logo to process_frame display
	const String logo = "./ADrive_Logo1.png";
	Mat mlogo = imread(logo, IMREAD_COLOR);

	// Resize the logo
	cv::resize(mlogo, ptr_server_obj->mat_logo, Size(90, 30));

	return 0;
}

/*
 ** main
 **
 ** Main Function
 */
int main(int argc, char* argv[])
{
	int tot_arg = argc;

	printf("\nargc: %d", argc);

	if (tot_arg < 5) {
		printf("***Error!! in arguments\n");
		printf("Usage: <gt_server_standalone> <IMU_filename> <Mapped_CRO_filename> <IMU_Image_folder_path> <save_odometry_data_filename>");
		return -1;
	}

	// structure define
	netrx serNet;

	// Command line Arguments pass into structure
	serNet.imu_filename      = argv[1];
	serNet.cro_filename      = argv[2];
	serNet.img_folder_name   = argv[3];
	serNet.odometry_filename = argv[4];

	// Data initialization
	Init(&serNet);

	// Load the IMU & CRO Data files & process the GT_D2L & ODO_D2L > Relocalization of car position 
	run_app(&serNet);

	return 0;
}