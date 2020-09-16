#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <sstream>
#include "json.hpp"

#pragma warning(disable:4996)

#include "opencv2/opencv.hpp"

#include "app_struct.h"
#include "packet.h"

using namespace std;
using namespace cv;

using json = nlohmann::json;

// Function Declaration
int run_car_relocalize_app(app_struct* ptr_struct_obj);

int process_video(app_struct* ptr_struct_obj);

/*
 ** init
 **
 ** Data initialization
 */
int init(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);
	
	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);

	// Variable initialization from structure
	string CRO_file = ptr_struct_obj->cro_filename;               // CRO_csv filename
	string IMU_file = ptr_struct_obj->imu_filename;               // IMU_csv filename
	string Odometry_filename = ptr_struct_obj->odometry_filename; // Odometry_csv filename

	char cro_filename[100];
	char imu_filename[100];

	const char* odometry_header_names = "FrameNumber,INS_Lat,INS_Lon,VO_D2L,Frame_status,GPS_status,VO_Lat,VO_Lon,VO_lat_Antenna_SL,VO_Lon_Antenna_SL,Confidence_SL, TS_Frame_no,TS_Shape,GT_lat_TS,GT_Lon_TS,VO_D2TS,VO_Lat_TS,VO_Lon_TS,Confidence_TS,VO_lat_Antenna_TS,VO_Lon_Antenna_TS\n";

	// Copying the contents of the string to char array to laod the .csv Files
	strcpy(cro_filename, IMU_file.c_str());
	strcpy(imu_filename, CRO_file.c_str());

	// Copy the string to the char *variable for FILE reading function (fopen)
	strcpy(ptr_struct_obj->odo_filename, Odometry_filename.c_str());

	printf("\nLoad the IMU_file...\n");

	// Open the IMU file
	ptr_struct_obj->imu_file = fopen(imu_filename, "r");

	// Coloumn counting counter
	int col_num_imu = 1;

	// For reading the row_data from csv file
	char line_imu[1024];

	// Read the Header line from the csv file to select the require columns by its header name
	fgets(line_imu, 1024, ptr_struct_obj->imu_file);

	// Store the coloumn inforamtion from the readed row_data
	const char* col_imu;

	// Based on coloumn Header string information, Give the coloumn number for extracting the perticular coloumn data
	for (col_imu = strtok(line_imu, ","); col_imu && *col_imu; col_imu = strtok(NULL, ",\n"))
	{
		printf("\n%s", col_imu);

		if (strcmp(col_imu, ptr_struct_obj->gps_time.c_str()) == 0)
		{
			ptr_struct_obj->time_col = col_num_imu;
			printf("\t= %d", ptr_struct_obj->time_col);
		}
		else if (strcmp(col_imu, ptr_struct_obj->ins_lat.c_str()) == 0) {

			ptr_struct_obj->lat_col = col_num_imu;
			printf("\t= %d", ptr_struct_obj->lat_col);
		}
		else if (strcmp(col_imu, ptr_struct_obj->ins_lon.c_str()) == 0) {

			ptr_struct_obj->lon_col = col_num_imu;
			printf("\t= %d", ptr_struct_obj->lon_col);
		}
		else if (strcmp(col_imu, ptr_struct_obj->frame_number_SL.c_str()) == 0) {

			ptr_struct_obj->frame_SL_no_col = col_num_imu;
			printf("\t= %d", ptr_struct_obj->frame_SL_no_col);
		}
		else if (strcmp(col_imu, ptr_struct_obj->frame_number_TS.c_str()) == 0) {

			ptr_struct_obj->frame_TS_no_col = col_num_imu;
			printf("\t= %d", ptr_struct_obj->frame_TS_no_col);
		}
		else if (strcmp(col_imu, ptr_struct_obj->gps_status.c_str()) == 0) {

			ptr_struct_obj->GPS_col = col_num_imu;
			printf("\t= %d", ptr_struct_obj->GPS_col);
		}
		else if (strcmp(col_imu, ptr_struct_obj->road.c_str()) == 0) {

			ptr_struct_obj->road_col = col_num_imu;
			printf("\t= %d", ptr_struct_obj->road_col);
		}
		else if (strcmp(col_imu, ptr_struct_obj->direction.c_str()) == 0) {

			ptr_struct_obj->direction_col = col_num_imu;
			printf("\t= %d", ptr_struct_obj->direction_col);
		}

		col_num_imu++;
	}
	
	printf("\n\nLoad the CRO_file...\n");

	// Open the CRO file
	ptr_struct_obj->cro_file = fopen(cro_filename, "r");

	// For reading the row_data from csv file
	char line_cro[1024];

	// Read the Header line from the csv file to select the require columns by its header name
	fgets(line_cro, 1024, ptr_struct_obj->cro_file);

	// Coloumn counting counter
	int col_num_cro = 1;

	// Store the coloumn inforamtion from the readed row_data
	const char* tok_cro;

	// Based on coloumn Header string information, Give the number coloumn for extracting the perticular coloumn data
	for (tok_cro = strtok(line_cro, ","); tok_cro && *tok_cro; tok_cro = strtok(NULL, ",\n"))
	{
		printf("\n%s", tok_cro);

		if (strcmp(tok_cro, ptr_struct_obj->gt_lat.c_str()) == 0)
		{
			ptr_struct_obj->gt_lat_col = col_num_cro;
			printf("\t= %d", ptr_struct_obj->gt_lat_col);
		}
		else if (strcmp(tok_cro, ptr_struct_obj->gt_lon.c_str()) == 0) {

			ptr_struct_obj->gt_lon_col = col_num_cro;
			printf("\t= %d", ptr_struct_obj->gt_lon_col);
		}
		else if (strcmp(tok_cro, ptr_struct_obj->gt_road_bearing.c_str()) == 0) {

			ptr_struct_obj->gt_bear_col = col_num_cro;
			printf("\t= %d\n", ptr_struct_obj->gt_bear_col);
		}
		else if (strcmp(tok_cro, ptr_struct_obj->gt_lat_TS.c_str()) == 0)
		{
			ptr_struct_obj->gt_lat_TS_col = col_num_cro;
			printf("\t= %d", ptr_struct_obj->gt_lat_TS_col);
		}
		else if (strcmp(tok_cro, ptr_struct_obj->gt_lon_TS.c_str()) == 0) {

			ptr_struct_obj->gt_lon_TS_col = col_num_cro;
			printf("\t= %d", ptr_struct_obj->gt_lon_TS_col);
		}
		else if (strcmp(tok_cro, ptr_struct_obj->gt_road_bearing_TS.c_str()) == 0) {

			ptr_struct_obj->gt_bear_TS_col = col_num_cro;
			printf("\t= %d\n", ptr_struct_obj->gt_bear_TS_col);
		}
		else if (strcmp(tok_cro, ptr_struct_obj->gt_width_TS.c_str()) == 0) {

			ptr_struct_obj->gt_width_TS_col = col_num_cro;
			printf("\t= %d", ptr_struct_obj->gt_width_TS_col);
		}
		else if (strcmp(tok_cro, ptr_struct_obj->gt_height_TS.c_str()) == 0) {

			ptr_struct_obj->gt_height_TS_col = col_num_cro;
			printf("\t= %d\n", ptr_struct_obj->gt_height_TS_col);
		}
		else if (strcmp(tok_cro, ptr_struct_obj->gt_shape_TS.c_str()) == 0) {

			ptr_struct_obj->gt_shape_TS_col = col_num_cro;
			printf("\t= %d\n", ptr_struct_obj->gt_shape_TS_col);
		}

		col_num_cro++;
	}
	
	// open the file and write the header line for saving the process Visual Odometry data
	if (ptr_struct_obj->vo_data_save_csv == VO_DATA_SAVE) {

		ptr_struct_obj->odometry_file = fopen(ptr_struct_obj->odo_filename, "a");
		
		fprintf(ptr_struct_obj->odometry_file, odometry_header_names);

		fclose(ptr_struct_obj->odometry_file);
	}

	if (ptr_struct_obj->vo_process_frames_video_save == VO_PROCESS_FRAME_SAVE) {

		// For Saving the process frames as video
		const String name = "./A7_S_7&8_sample.avi";

		ptr_struct_obj->wrOutVideo.open(name, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, Size(640, 480), true);
	}

	if (ptr_struct_obj->adrive_logo == ADRIVE_LOGO) {

		// For add the logo to process_frame display
		const String logo = "./ADrive_Logo1.png";
		Mat mlogo = imread(logo, IMREAD_COLOR);

		// Resize the logo
		cv::resize(mlogo, ptr_struct_obj->mat_logo_SL, Size(90, 30));   // For the D2SL frame
		cv::resize(mlogo, ptr_struct_obj->mat_logo_TS, Size(135, 50));  // For the D2TS Frame
	}

	return 0;
}

/*
 ** deInit
 **
 ** Data de-initialization
 */
int deInit(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);
	
	// IMU file Close
	fclose(ptr_struct_obj->imu_file);
	
	// CRO file Close
	fclose(ptr_struct_obj->cro_file);
	
	return 0;
}

int parse_config(app_struct* ptr_struct_obj)
{
	// Load the config.json file for parsing the information
	ifstream configFile("./config.txt");

	// TODO if config file is not found, set the values to default
	
	// check the config file is open or not
	if (configFile.fail()) {

		printf("***Error:Config file is not found!");
		return -1;
	}
	
	json j = json::parse(configFile);

	// Parse the json strings to the Structure variables

	// Assign the filenames of needed Input .csv files (IMU & CRO), IMU Image folder path and
	// Output file (.csv) info to the structure variables

	ptr_struct_obj->imu_filename           = j["imu_file"].get_ref<const string&>();

	ptr_struct_obj->cro_filename           = j["mapped_cro_File"].get_ref<const string&>();

	ptr_struct_obj->img_folder_name        = j["image_folder_path"].get_ref<const string&>();

	ptr_struct_obj->Process_video_filename = j["process_video_filename"].get_ref<const string&>();

	ptr_struct_obj->odometry_filename      = j["output_odometry"].get_ref<const string&>();
	
	// Assign the Actual GPS Mounting position offset with referece to FR Wheel info to the structure variables

    // TODO check for stof error
	ptr_struct_obj->gps_lat_offset = stof(j["lat_offset"].get_ref<const string&>());
	
	ptr_struct_obj->gps_lon_offset = stof(j["lon_offset"].get_ref<const string&>());

	// Assign the needed Input .csv file Coloumn Header string info to the structure variables

	ptr_struct_obj->gps_time           = j["gps_time"].get_ref<const string&>();

	ptr_struct_obj->ins_lat            = j["ins_lat"].get_ref<const string&>();

	ptr_struct_obj->ins_lon            = j["ins_lon"].get_ref<const string&>();

	ptr_struct_obj->frame_number_SL    = j["frame_number_SL"].get_ref<const string&>();
	
	ptr_struct_obj->frame_number_TS    = j["frame_number_TS"].get_ref<const string&>();

	ptr_struct_obj->gps_status         = j["gps_status"].get_ref<const string&>();

	ptr_struct_obj->road               = j["road"].get_ref<const string&>();

	ptr_struct_obj->direction          = j["direction"].get_ref<const string&>();

	ptr_struct_obj->gt_lat             = j["gt_lat"].get_ref<const string&>();

	ptr_struct_obj->gt_lon             = j["gt_lon"].get_ref<const string&>();

	ptr_struct_obj->gt_road_bearing    = j["gt_road_bearing"].get_ref<const string&>();

	ptr_struct_obj->gt_lat_TS          = j["gt_lat_TS"].get_ref<const string&>();

	ptr_struct_obj->gt_lon_TS          = j["gt_lon_TS"].get_ref<const string&>();

	ptr_struct_obj->gt_road_bearing_TS = j["gt_road_bearing_TS"].get_ref<const string&>();

	ptr_struct_obj->gt_width_TS        = j["gt_width_TS"].get_ref<const string&>();

	ptr_struct_obj->gt_height_TS       = j["gt_height_TS"].get_ref<const string&>();

	ptr_struct_obj->gt_shape_TS        = j["gt_shape_TS"].get_ref<const string&>();

	// Assign the Camera Calibration P-arameters to the structure variables

    // TODO check for stoi error
	ptr_struct_obj->start_X      = stoi(j["cali_START_X"].get_ref<const string&>());

	ptr_struct_obj->start_Y      = stoi(j["cali_START_Y"].get_ref<const string&>());

	ptr_struct_obj->end_Y        = stoi(j["cali_END_Y"].get_ref<const string&>());

	ptr_struct_obj->frame_width  = stoi(j["frame_width"].get_ref<const string&>());

	ptr_struct_obj->pixel_dist   = stof(j["pixel_distance"].get_ref<const string&>());

	ptr_struct_obj->cali_max_D2L = stof(j["cali_max_D2L"].get_ref<const string&>());

	// Assign the Visual Odometry data saving format option to the structure variables

	ptr_struct_obj->relocalisation_app           = stoi(j["Relocalisation_app"].get_ref<const string&>());

	ptr_struct_obj->process_video_D2SL           = stoi(j["process_video_D2SL"].get_ref<const string&>());

	ptr_struct_obj->vo_data_save_csv             = stoi(j["vo_data_save"].get_ref<const string&>());

	ptr_struct_obj->vo_process_frame_save        = stoi(j["process_frame_save"].get_ref<const string&>());

	ptr_struct_obj->vo_process_frames_video_save = stoi(j["process_frames_video_save"].get_ref<const string&>());

	ptr_struct_obj->traffic_info                 = stoi(j["traffic_info"].get_ref<const string&>());

	ptr_struct_obj->adrive_logo                  = stoi(j["adrive_logo"].get_ref<const string&>());
	
	ptr_struct_obj->frame_process_delay_ms       = stoi(j["frame_process_delay_ms"].get_ref<const string&>());

	// Assign the ROI parameters for process the direct Video to D2L without GT Info

	ptr_struct_obj->roi_x1 = stoi(j["roi_x1"].get_ref<const string&>());

	ptr_struct_obj->roi_y1 = stoi(j["roi_y1"].get_ref<const string&>());

	ptr_struct_obj->roi_x2 = stoi(j["roi_x2"].get_ref<const string&>());

	ptr_struct_obj->roi_y2 = stoi(j["roi_y2"].get_ref<const string&>());

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
	app_struct app_packet;
	
	// TODO init the app structure, else it will have garbage values

	// read and parse the json string to the code as user input
	parse_config(&app_packet);

	if ((app_packet.relocalisation_app == RUN_RELOCALISATION_APP)) {

		// Data initialization
		init(&app_packet);

		// Run the Car Reloclize Aplication
		run_car_relocalize_app(&app_packet);

		// De-Initialization
		deInit(&app_packet);
	}

	if ((app_packet.process_video_D2SL == RUN_VIDEO_PROCESS_D2SL_APP)) {

		process_video(&app_packet);
	}

	return 0;
}