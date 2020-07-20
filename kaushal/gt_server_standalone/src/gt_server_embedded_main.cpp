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
	char* CRO_filename     = ptr_server_obj->cro_filename;       // CRO_csv filename
	char* IMU_filename     = ptr_server_obj->imu_filename;       // IMU_csv filename
	char* IMG_folder_name  = ptr_server_obj->img_folder_name;    // IMU_IMG_Folder path
	char* ODO_filename     = ptr_server_obj->odometry_filename;  // Odometry_csv filename
	
	float roadBearing_rot;
	int frame_status;
	
	// For load the IMU_Frame
	Mat frame;

	// File for Update the Relocalize Data
	FILE* ODO_file;
	
	// For store the readed line data from csv file (IMU & CRO)
	string row_imu;
	string row_cro;
	
	// Values from csv files > string to specific variables
	int time;
	int frame_imu1;
	float roadBearing;

	printf("\nLoad the IMU_file...\n");
	// Open the GT_METADA csv file for create the ROI for D2L processing in frame
	ifstream  imu_data(IMU_filename);
	
	printf("\nLoad the CRO_file...\n");
	// Open the GT_METADA csv file for create the ROI for D2L processing in frame
	ifstream  cro_data(CRO_filename);

	// Skip the header line or first line of IMU_file
	getline(imu_data, row_imu);

	// Skip the header line or first line of CRO_file
	getline(cro_data, row_cro);

	// open the file and write the header line for saving the process frame data
	if (RELOCALIZE_DATA_SAVE == 1) {

		ODO_file = fopen(ODO_filename, "a");
  	    fprintf(ODO_file, "INS_time_ms,INS_Lat,INS_Lon,Nearest_Lat,Nearest_Lon,GT_D2L_m,Frame_no,ODO_D2L_m,Lat_new,Lon_new,GPS_stauts,Frame_status,Confidence,INS_lat_antenna,INS_lon_antenna,Validate\n");

	    fclose(ODO_file);
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
		
		if (!frame_imu.empty() && !time_imu.empty() && !lat_imu.empty() && !lon_imu.empty()) {
			
			stringstream  row_stream(row_cro);
			string gps_imu_cro;
			string time_imu_cro;
			string lat_imu_cro;
			string lon_imu_cro;
			string frame_imu_cro;
			string road_cro;
			string direction_cro;
			string GT_D2l_cro;
			string lat_cro;
			string lon_cro;
			string bearing_cro;
			
			// extract the words from row in to columns in sequence
			getline(row_stream, gps_imu_cro, ',');
			getline(row_stream, time_imu_cro, ',');
			getline(row_stream, lat_imu_cro, ',');
			getline(row_stream, lon_imu_cro, ',');
			getline(row_stream, frame_imu_cro, ',');
			getline(row_stream, road_cro, ',');
			getline(row_stream, direction_cro, ',');
			getline(row_stream, GT_D2l_cro, ',');
			getline(row_stream, lat_cro, ',');
			getline(row_stream, lon_cro, ',');
			getline(row_stream, bearing_cro, ',');
			
			// check, if the GT_data is not available or not
			if ((lat_cro == "None") || (lon_cro == "None")) {

				if ((RELOCALIZE_DATA_SAVE == 1)) {

					frame_status = 0;

					// open the file for saving the process frame data at the end of the loop
					ODO_file = fopen(ODO_filename, "a");

					fprintf(ODO_file, "%d,%.8f,%.8f,None,None,None,%d,None,None,None,%d,%d,None\n"
						, stoi(time_imu), stod(lat_imu), stod(lon_imu), stoi(frame_imu), stoi(gps_imu), frame_status);

					fclose(ODO_file);
				}

			} else {

				// convert the IMU data from string to numbers
				ptr_metadata->u4_timestampL     = stoi(time_imu);
				ptr_metadata->u4_ins_gps_status = stoi(gps_imu);
				ptr_metadata->d8_ins_latitude   = stod(lat_imu);
				ptr_metadata->d8_ins_longitude  = stod(lon_imu);
				ptr_metadata->u4_frame_number   = stoi(frame_imu);
				
				ptr_server_obj->road            = road_cro;
				ptr_server_obj->direction       = direction_cro;

				printf("\n\nFrame %d: %d\t%.8f\t%.8f\n"
					, ptr_metadata->u4_frame_number, ptr_metadata->u4_timestampL, ptr_metadata->d8_ins_latitude
					, ptr_metadata->d8_ins_longitude);

				// convert the CRO data from string to numbers
				ptr_gtMetadata->d8_gt_latitude = stod(lat_cro);
				ptr_gtMetadata->d8_gt_longitude = stod(lon_cro);
				roadBearing = stof(bearing_cro);
				
				printf("\nCRO %d: %.8f\t%.8f\n", ptr_metadata->u4_frame_number, ptr_gtMetadata->d8_gt_latitude
					, ptr_gtMetadata->d8_gt_longitude);

				// calculate the distance (GT_D2L) betwwen two points IMU_lat_lon & GT_lat_lon > bearing angle
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
				
				// process the IMU_Frame fro ODO_D2L to relocalize the car position
				process_frame(ptr_server_obj);
				
				// Bearing angle rotarte to lateral Axis to calculate the new Lat & Lon for car Re-localization
				roadBearing_rot = bearing_angle90deg(roadBearing);

				//find the new car Posiotion Lat_new, Lon_new
				LandmarkOdometry(ptr_server_obj, roadBearing_rot);
				
				// Relocalize_GPS (Front Right Tyre) Correction to the actual GPS_mounting offset
				gps_mounting_offset(ptr_server_obj);
				
				// If Traffic info or Frame is available then also Relcoalize the car using D2TS
				if (TRAFFIC_INFO == 1)
				{
					run_traffic_sign_relocalize_app(ptr_server_obj, roadBearing);
				}
				
				// Calculate the Confidence and save the Relocalize data as .csv file
				if ((RELOCALIZE_DATA_SAVE == 1)) {
					
					// open the file for saving the process frame data at the end of the loop
					ODO_file = fopen(ODO_filename, "a");

					ptr_server_obj->odometry_file = ODO_file;

					odometry_data_save(ptr_server_obj);

					fclose(ODO_file);
				}
				
				if (PROCESS_FRAME_SAVE == 1) {

					// To create the filenames as imu_frame number for save the process frames
					std::stringstream ss;
					ss << "Data/img_process/" << ptr_metadata->u4_frame_number << ".png";

					// save process_frame as imag
					imwrite(ss.str().c_str(), frame);
				}

				waitKey(1);
			}
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

/*
 ** Init
 **
 ** Data initialization
 */
int init(netrx *ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);
	
	if (PROCESS_FRAMES_VIDEO_SAVE == 1) {

		// For Saving the process frames as video
		const String name = "./A7_24.04.2020_7&8.avi";

		ptr_server_obj->wrOutVideo.open(name, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, Size(640, 480), true);
	}

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
	init(&serNet);
	
	// Run the Car Reloclize Aplication
	run_car_relocalize_app(&serNet);
	
	return 0;
}