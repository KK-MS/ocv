#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <sstream>
#include "json.hpp"

#pragma warning(disable:4996)

#include "opencv2/opencv.hpp"

#include "netrx.h"
#include "packet.h"

using namespace std;
using namespace cv;

using json = nlohmann::json;

// Function Declaration
int run_car_relocalize_app(netrx* ptr_server_obj);

/*
 ** init
 **
 ** Data initialization
 */
int init(netrx *ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);
	
	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	// Variable initialization from structure
	string CRO_file = ptr_server_obj->cro_filename;               // CRO_csv filename
	string IMU_file = ptr_server_obj->imu_filename;               // IMU_csv filename
	string Odometry_filename = ptr_server_obj->odometry_filename; // Odometry_csv filename

	char cro_filename[100];
	char imu_filename[100];

	// Copying the contents of the string to char array to laod the .csv Files
	strcpy(cro_filename, IMU_file.c_str());
	strcpy(imu_filename, CRO_file.c_str());

	// Copy the string to the char *variable for FILE reading function (fopen)
	strcpy(ptr_server_obj->ODO_filename, Odometry_filename.c_str());

	printf("\nLoad the IMU_file...\n");

	// Open the IMU file
	ptr_server_obj->imu_file = fopen(imu_filename, "r");

	// Coloumn counting counter
	int col_num_imu = 1;

	// For reading the row_data from csv file
	char line_imu[1024];

	// Read the Header line from the csv file to select the require columns by its header name
	fgets(line_imu, 1024, ptr_server_obj->imu_file);

	// Store the coloumn inforamtion from the readed row_data
	const char* col_imu;

	// Based on coloumn Header string information, Give the coloumn number for extracting the perticular coloumn data
	for (col_imu = strtok(line_imu, ","); col_imu && *col_imu; col_imu = strtok(NULL, ",\n"))
	{
		printf("\n%s", col_imu);

		if (strcmp(col_imu, ptr_server_obj->GPS_Time.c_str()) == 0)
		{
			ptr_server_obj->time_col = col_num_imu;
			printf("\t= %d", ptr_server_obj->time_col);

		}
		else if (strcmp(col_imu, ptr_server_obj->INS_Lat.c_str()) == 0) {

			ptr_server_obj->lat_col = col_num_imu;
			printf("\t= %d", ptr_server_obj->lat_col);
		}
		else if (strcmp(col_imu, ptr_server_obj->INS_Lon.c_str()) == 0) {

			ptr_server_obj->lon_col = col_num_imu;
			printf("\t= %d", ptr_server_obj->lon_col);
		}
		else if (strcmp(col_imu, ptr_server_obj->frame_number.c_str()) == 0) {

			ptr_server_obj->frame_no_col = col_num_imu;
			printf("\t= %d", ptr_server_obj->frame_no_col);
		}
		else if (strcmp(col_imu, ptr_server_obj->GPS_status.c_str()) == 0) {

			ptr_server_obj->GPS_col = col_num_imu;
			printf("\t= %d", ptr_server_obj->GPS_col);
		}
		else if (strcmp(col_imu, ptr_server_obj->Road.c_str()) == 0) {

			ptr_server_obj->road_col = col_num_imu;
			printf("\t= %d", ptr_server_obj->road_col);
		}
		else if (strcmp(col_imu, ptr_server_obj->Direction.c_str()) == 0) {

			ptr_server_obj->direction_col = col_num_imu;
			printf("\t= %d", ptr_server_obj->direction_col);
		}

		col_num_imu++;
	}
	
	printf("\n\nLoad the CRO_file...\n");

	// Open the CRO file
	ptr_server_obj->cro_file = fopen(cro_filename, "r");

	// For reading the row_data from csv file
	char line_cro[1024];

	// Read the Header line from the csv file to select the require columns by its header name
	fgets(line_cro, 1024, ptr_server_obj->cro_file);

	// Coloumn counting counter
	int col_num_cro = 1;

	// Store the coloumn inforamtion from the readed row_data
	const char* tok_cro;

	// Based on coloumn Header string information, Give the number coloumn for extracting the perticular coloumn data
	for (tok_cro = strtok(line_cro, ","); tok_cro && *tok_cro; tok_cro = strtok(NULL, ",\n"))
	{
		printf("\n%s", tok_cro);

		if (strcmp(tok_cro, ptr_server_obj->GT_lat.c_str()) == 0)
		{
			ptr_server_obj->GT_lat_col = col_num_cro;
			printf("\t= %d", ptr_server_obj->GT_lat_col);
		}
		else if (strcmp(tok_cro, ptr_server_obj->GT_lon.c_str()) == 0) {

			ptr_server_obj->GT_lon_col = col_num_cro;
			printf("\t= %d", ptr_server_obj->GT_lon_col);
		}
		else if (strcmp(tok_cro, ptr_server_obj->GT_road_bearing.c_str()) == 0) {

			ptr_server_obj->GT_bear_col = col_num_cro;
			printf("\t= %d\n", ptr_server_obj->GT_bear_col);
		}

		col_num_cro++;
	}
	
	// open the file and write the header line for saving the process Visual Odometry data
	if (ptr_server_obj->VO_data_save_csv == 1) {

		ptr_server_obj->odometry_file = fopen(ptr_server_obj->ODO_filename, "a");

		fprintf(ptr_server_obj->odometry_file, "INS_time_ms,INS_Lat,INS_Lon,Nearest_Lat,Nearest_Lon,GT_D2L_m,Frame_no,ODO_D2L_m,Lat_new,Lon_new,GPS_stauts,Frame_status,Confidence,INS_lat_antenna,INS_lon_antenna\n");

		fclose(ptr_server_obj->odometry_file);
	}

	if (ptr_server_obj->VO_process_frames_video_save == 1) {

		// For Saving the process frames as video
		const String name = "./A7_S_7&8_sample.avi";

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
 ** deInit
 **
 ** Data de-initialization
 */
int deInit(netrx* ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);
	
	// IMU file Close
	fclose(ptr_server_obj->imu_file);
	
	// CRO file Close
	fclose(ptr_server_obj->cro_file);
	
	return 0;
}

/*
 ** main
 **
 ** Main Function
 */
int main(int argc, char* argv[])
{
	// structure define
	netrx serNet;
	
	// Load the config.json file for parsing the information
	ifstream jfile("./config.txt");
	
	json j = json::parse(jfile);
	
	// Parse the json strings to the Structure variables

	// Assign the filenames of needed Input .csv files (IMU & CRO), IMU Image folder path and
	// Output file (.csv) info to the structure variables

	serNet.imu_filename = j["IMU_File"].get_ref<const string&>();
	
	serNet.cro_filename = j["Mapped_CRO_File"].get_ref<const string&>();
		
	serNet.img_folder_name = j["Image_Folder_path"].get_ref<const string&>();
	
	serNet.odometry_filename = j["Output_odometry"].get_ref<const string&>();
	
	// Assign the Actual GPS Mounting position offset with referece to FR Wheel info to the structure variables

	serNet.gps_lat_offset = stof(j["Lat_offset"].get_ref<const string&>());
	
	serNet.gps_lon_offset = stof(j["Lon_offset"].get_ref<const string&>());
		
	// Assign the needed Input .csv file Coloumn Header string info to the structure variables

	serNet.GPS_Time        = j["GPS_Time"].get_ref<const string&>();

	serNet.INS_Lat         = j["INS_Lat"].get_ref<const string&>();

	serNet.INS_Lon         = j["INS_Lon"].get_ref<const string&>();

	serNet.frame_number    = j["frame_number"].get_ref<const string&>();

	serNet.GPS_status      = j["GPS_status"].get_ref<const string&>();

	serNet.Road            = j["Road"].get_ref<const string&>();

	serNet.Direction       = j["Direction"].get_ref<const string&>();

	serNet.GT_lat          = j["GT_lat"].get_ref<const string&>();

	serNet.GT_lon          = j["GT_lon"].get_ref<const string&>();

	serNet.GT_road_bearing = j["GT_road_bearing"].get_ref<const string&>();

	// Assign the Camera Calibration P-arameters to the structure variables

	serNet.start_X         = stoi(j["Cali_START_X"].get_ref<const string&>());

	serNet.start_Y         = stoi(j["Cali_START_Y"].get_ref<const string&>());

	serNet.end_Y           = stoi(j["Cali_END_Y"].get_ref<const string&>());

	serNet.frame_width     = stoi(j["Frame_width"].get_ref<const string&>());

	serNet.pixel_dist      = stof(j["Pixel_distance"].get_ref<const string&>());

	serNet.cali_max_D2L    = stof(j["Cali_max_D2L"].get_ref<const string&>());

	// Assign the Visual Odometry data saving format option to the structure variables

	serNet.VO_data_save_csv             = stoi(j["VO_Data_Save"].get_ref<const string&>());
	
	serNet.VO_process_frame_save        = stoi(j["Process_Frame_Save"].get_ref<const string&>());
	
	serNet.VO_process_frames_video_save = stoi(j["Process_Frames_video_SAVE"].get_ref<const string&>());
	
	serNet.traffic_info                 = stoi(j["Traffic_info"].get_ref<const string&>());
	
	// Data initialization
	init(&serNet);
	
	// Run the Car Reloclize Aplication
	run_car_relocalize_app(&serNet);
	
	// De-Initialization
	deInit(&serNet);

	return 0;
}