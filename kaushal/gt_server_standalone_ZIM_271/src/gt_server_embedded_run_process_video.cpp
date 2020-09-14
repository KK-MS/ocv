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
int process_frame_D2SL(app_struct* ptr_struct_obj);


/*
 ** run_side_lane_relocalize_app
 **
 ** Load the IMU & CRO Data files & process the GT_D2L & ODO_D2L
 ** Relocalization of car position using Side lane
 */
int process_video(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);
	
	// Variable initialization from structure
	string Odometry_filename = ptr_struct_obj->odometry_filename; // Odometry_csv filename	
	
	// Copy the string to the char *variable for FILE reading function (fopen)
	strcpy(ptr_struct_obj->ODO_filename, Odometry_filename.c_str());

	int iFrameCount = 0;
	float ODO_D2SL;

	// load the video
	VideoCapture cap(ptr_struct_obj->Process_video_filename);

	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	// find the Frame height and width
	int width = cap.get(CAP_PROP_FRAME_WIDTH);   // Captured video frame width in pixels
	int height = cap.get(CAP_PROP_FRAME_HEIGHT);  // Captured video frame height in pixels

	printf("\nframe_Height: %d \nframe_width: %d \n\n", height, width);

	double fps = cap.get(CAP_PROP_FPS); //get the frames per seconds of the video

	cout << "Frame per seconds : " << fps << endl;

	// For add the logo to process_frame display
	const String logo = "./ADrive_Logo1.png";
	Mat mlogo = imread(logo, IMREAD_COLOR);

	// Resize the logo For 1280 x 720 frame
	cv::resize(mlogo, ptr_struct_obj->mat_logo_SL, Size(135, 50));

	if ((ptr_struct_obj->VO_data_save_csv == 1)) {

		// open the file for saving the process frame data at the end of the loop
		ptr_struct_obj->odometry_file = fopen(ptr_struct_obj->ODO_filename, "a");

		// Write the process METADATA in .csv file
		fprintf(ptr_struct_obj->odometry_file, "Frame_No, odo_D2SL\n");

		fclose(ptr_struct_obj->odometry_file);
	}

	if (ptr_struct_obj->VO_process_frames_video_save == 1) {

		// For Saving the process frames as video
		const String process_video_filename = "data/GH015950_process.avi";

		ptr_struct_obj->wrOutVideo.open(process_video_filename, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, Size(1280, 720), true);
	}
	
	// Process the frames to find D2L and save the processed frames in device
	while (1)
	{
		// Create the frame
		ptr_struct_obj->frame_SL = Mat::zeros(height, width, CV_8UC3);

		// Capture frame-by-frame
		cap >> ptr_struct_obj->frame_SL;

		// pass the frame number in to structure
		ptr_metadata->u4_frame_number = iFrameCount;

		printf("\nFrame: % d", iFrameCount);

		// If the frame is empty, break immediately
		if (!ptr_struct_obj->frame_SL.empty()) {

			// Process the IMU_Frame fro ODO_D2L to relocalize the car position
			process_frame_D2SL(ptr_struct_obj);

			// take the calculated D2SL from structure
			ODO_D2SL = ptr_metadata->f4_odo_distance;  // ODO_D2SL

			// Calculate the Confidence and save the Relocalize data as .csv file
			if ((ptr_struct_obj->VO_data_save_csv == 1) && ODO_D2SL > 0.12) {

				// open the file for saving the process frame data at the end of the loop
				ptr_struct_obj->odometry_file = fopen(ptr_struct_obj->ODO_filename, "a");

				// Write the process METADATA in .csv file
				fprintf(ptr_struct_obj->odometry_file, "%d, %f\n", iFrameCount, ODO_D2SL);

				fclose(ptr_struct_obj->odometry_file);
			}

			waitKey(1);
		}
		else {

			continue;
		}

		iFrameCount ++;
	}
	return 0;
}

