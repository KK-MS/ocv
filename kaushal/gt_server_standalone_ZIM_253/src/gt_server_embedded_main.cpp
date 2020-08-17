#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <sstream>
#include "json.hpp"

//#include <json-c/json.h>
//#include <windows.data.json.h>
//#pragma comment(lib, "runtimeobject.lib")

#pragma warning(disable:4996)

#include "opencv2/opencv.hpp"

#include "netrx.h"
#include "packet.h"

//using namespace ABI::Windows::Data::Json;

using namespace std;
using namespace cv;

//using namespace restc_cpp;
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
	
	if (PROCESS_FRAMES_VIDEO_SAVE == 1) {

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
	
	// Parse the json strings (Command line Arguments) to the Structure
	ifstream jfile("./config.json");
		
	json j = json::parse(jfile);

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

	// Data initialization
	init(&serNet);
	
	// Run the Car Reloclize Aplication
	run_car_relocalize_app(&serNet);

	return 0;
}