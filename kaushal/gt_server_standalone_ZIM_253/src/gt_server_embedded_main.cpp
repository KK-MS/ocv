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
 ** Init
 **
 ** Data initialization
 */
int init(netrx *ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);
	
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
 ** main
 **
 ** Main Function
 */
int main(int argc, char* argv[])
{
	// structure define
	netrx serNet;
	
	// Load the config.json file for parsing the information
	ifstream jfile("./config.json");
	
	json j = json::parse(jfile);
	
	// Parse the json strings to the Structure variables

	serNet.imu_filename = j["IMU_File"].get_ref<const std::string&>();
	std::cout << "IMU_File: " << serNet.imu_filename << std::endl;

	serNet.cro_filename = j["Mapped_CRO_File"].get_ref<const std::string&>();
	std::cout << "CRO_File: " << serNet.cro_filename << std::endl;

	serNet.img_folder_name = j["Image_Folder_path"].get_ref<const std::string&>();
	std::cout << "Image_Folder_path: " << serNet.img_folder_name << std::endl;

	serNet.odometry_filename = j["Output_odometry"].get_ref<const std::string&>();
	std::cout << "Odometry_filename: " << serNet.odometry_filename << std::endl;

	serNet.gps_lat_offset = stof(j["Lat_offset"].get_ref<const std::string&>());
	std::cout << "GPS_mo_Lat_offset: " << serNet.gps_lat_offset << std::endl;

	serNet.gps_lon_offset = stof(j["Lon_offset"].get_ref<const std::string&>());
	std::cout << "GPS_mo_Lon_offset: " << serNet.gps_lon_offset << std::endl;
	
	serNet.GPS_Time        = j["GPS_Time"].get_ref<const std::string&>();

	serNet.INS_Lat         = j["INS_Lat"].get_ref<const std::string&>();

	serNet.INS_Lon         = j["INS_Lon"].get_ref<const std::string&>();

	serNet.frame_number    = j["frame_number"].get_ref<const std::string&>();

	serNet.GPS_status      = j["GPS_status"].get_ref<const std::string&>();

	serNet.Road            = j["Road"].get_ref<const std::string&>();

	serNet.Direction       = j["Direction"].get_ref<const std::string&>();

	serNet.GT_lat          = j["GT_lat"].get_ref<const std::string&>();

	serNet.GT_lon          = j["GT_lon"].get_ref<const std::string&>();

	serNet.GT_road_bearing = j["GT_road_bearing"].get_ref<const std::string&>();

	serNet.start_X         = stoi(j["Cali_START_X"].get_ref<const std::string&>());

	serNet.start_Y         = stoi(j["Cali_START_Y"].get_ref<const std::string&>());

	serNet.end_Y           = stoi(j["Cali_END_Y"].get_ref<const std::string&>());

	serNet.frame_width     = stoi(j["Frame_width"].get_ref<const std::string&>());

	serNet.pixel_dist      = stof(j["Pixel_distance"].get_ref<const std::string&>());

	serNet.cali_max_D2L    = stof(j["Cali_max_D2L"].get_ref<const std::string&>());

	serNet.VO_data_save_csv             = stoi(j["VO_Data_Save"].get_ref<const std::string&>());
	
	serNet.VO_process_frame_save        = stoi(j["Process_Frame_Save"].get_ref<const std::string&>());
	
	serNet.VO_process_frames_video_save = stoi(j["Process_Frames_video_SAVE"].get_ref<const std::string&>());
	
	serNet.traffic_info                 = stoi(j["Traffic_info"].get_ref<const std::string&>());
		
	// Data initialization
	init(&serNet);
	
	// Run the Car Reloclize Aplication
	run_car_relocalize_app(&serNet);

	return 0;
}