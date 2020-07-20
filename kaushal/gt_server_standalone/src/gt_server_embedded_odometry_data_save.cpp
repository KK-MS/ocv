#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>

#pragma warning(disable:4996)

#include "netrx.h"
#include "packet.h"

using namespace std;
using namespace cv;

#define ALL_DATA_SAVE  1
#define FRAME_STATUS   0

/*
 ** odometry_data_save
 **
 ** Saving the Process odometry data
 */
int odometry_data_save(netrx *ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	// Variable initialization from structure
	unsigned int time  = ptr_metadata->u4_timestampL;      // IMU_time
	float GT_D2L       = ptr_gtMetadata->f4_gt_distance;   // GT_D2L
	float ODO_D2L      = ptr_metadata->f4_odo_distance;    // ODO_D2L
	FILE* ODO_file     = ptr_server_obj->odometry_file;    // file for Update the Relocalize Data

	// Calculate the confidence value between GT_D2L & ODO_D2L
	float confidence = GT_D2L - ODO_D2L;

#if ALL_DATA_SAVE

	if ((ODO_D2L < 0.9)) {

		// print the data in odometry.csv for analysis or future use
		fprintf(ODO_file, "%d,%.8f,%.8f,%.8f,%.8f,%f,%d,%f,%.8f,%.8f,%d,%d,%f,%.8f,%.8f,%f\n"
			, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			, ptr_gtMetadata->d8_gt_latitude, ptr_gtMetadata->d8_gt_longitude
			, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			, ptr_metadata->f4_odo_distance, ptr_metadata->d8_out_odo_latitude
			, ptr_metadata->d8_out_odo_longitude, ptr_metadata->u4_ins_gps_status
			, FRAME_STATUS_8, confidence, ptr_metadata->d8_mo_ins_latitude_antenna
			, ptr_metadata->d8_mo_ins_longitude_antenna, ptr_metadata->f4_mo_distance);
	}
	else if ((ODO_D2L > 0.9) || (ODO_D2L <= 0)) {

		// print the data in odometry.csv for analysis or future use
		fprintf(ODO_file, "%d,%.8f,%.8f,%.8f,%.8f,%f,%d,%f,%.8f,%.8f,%d,%d,%f,%.8f,%.8f,%f\n"
			, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			, ptr_gtMetadata->d8_gt_latitude, ptr_gtMetadata->d8_gt_longitude
			, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			, ptr_metadata->f4_odo_distance, ptr_metadata->d8_out_odo_latitude
			, ptr_metadata->d8_out_odo_longitude, ptr_metadata->u4_ins_gps_status
			, FRAME_STATUS_0, confidence, ptr_metadata->d8_mo_ins_latitude_antenna
			, ptr_metadata->d8_mo_ins_longitude_antenna, ptr_metadata->f4_mo_distance);
	}

#endif

#if FRAME_STATUS

	if ((ptr_metadata->u4_ins_gps_status == 8) && (ODO_D2L < 0.9) && ((confidence < 0.15) || (confidence < -0.15))) {
		
		// print the data in odometry.csv for analysis or future use
		fprintf(ODO_file, "%d,%.8f,%.8f,%.8f,%.8f,%f,%d,%f,%.8f,%.8f,%d,%d,%f\n"
			, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			, ptr_gtMetadata->d8_gt_latitude, ptr_gtMetadata->d8_gt_longitude
			, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			, ptr_metadata->f4_odo_distance, ptr_metadata->d8_out_odo_latitude
			, ptr_metadata->d8_out_odo_longitude, ptr_metadata->u4_ins_gps_status
			, FRAME_STATUS_8, confidence);
	}
	else if ((ptr_metadata->u4_ins_gps_status == 8) && (ODO_D2L < 0.9) && ((confidence < 0.30) || (confidence < -0.30))) {
		
		// print the data in odometry.csv for analysis or future use
		fprintf(ODO_file, "%d,%.8f,%.8f,%.8f,%.8f,%f,%d,%f,%.8f,%.8f,%d,%d,%f\n"
			, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			, ptr_gtMetadata->d8_gt_latitude, ptr_gtMetadata->d8_gt_longitude
			, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			, ptr_metadata->f4_odo_distance, ptr_metadata->d8_out_odo_latitude
			, ptr_metadata->d8_out_odo_longitude, ptr_metadata->u4_ins_gps_status
			, FRAME_STATUS_4, confidence);
	}
	else if ((ptr_metadata->u4_ins_gps_status == 8) && (ODO_D2L < 0.9) && ((confidence < 0.5) || (confidence < -0.5))) {
		
		// print the data in odometry.csv for analysis or future use
		fprintf(ODO_file, "%d,%.8f,%.8f,%.8f,%.8f,%f,%d,%f,%.8f,%.8f,%d,%d,%f\n"
			, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			, ptr_gtMetadata->d8_gt_latitude, ptr_gtMetadata->d8_gt_longitude
			, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			, ptr_metadata->f4_odo_distance, ptr_metadata->d8_out_odo_latitude
			, ptr_metadata->d8_out_odo_longitude, ptr_metadata->u4_ins_gps_status
			, FRAME_STATUS_2, confidence);
	}
	else if ((ptr_metadata->u4_ins_gps_status == 8) && (ODO_D2L < 0.9) && ((confidence < 0.9) || (confidence < -0.9))) {

		// print the data in odometry.csv for analysis or future use
		fprintf(ODO_file, "%d,%.8f,%.8f,%.8f,%.8f,%f,%d,%f,%.8f,%.8f,%d,%d,%f\n"
			, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			, ptr_gtMetadata->d8_gt_latitude, ptr_gtMetadata->d8_gt_longitude
			, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			, ptr_metadata->f4_odo_distance, ptr_metadata->d8_out_odo_latitude
			, ptr_metadata->d8_out_odo_longitude, ptr_metadata->u4_ins_gps_status
			, FRAME_STATUS_1, confidence);
	}
	else if ((GT_D2L > 0.9) || (ODO_D2L > 0.9)) {

		// print the data in odometry.csv for analysis future use
		fprintf(ODO_file, "%d,%.8f,%.8f,%.8f,%.8f,%f,%d,%f,%.8f,%.8f,%d,%d,%f\n"
			, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			, ptr_gtMetadata->d8_gt_latitude, ptr_gtMetadata->d8_gt_longitude
			, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			, ptr_metadata->f4_odo_distance, ptr_metadata->d8_out_odo_latitude
			, ptr_metadata->d8_out_odo_longitude, ptr_metadata->u4_ins_gps_status
			, FRAME_STATUS_0, confidence);
	}
#endif

	return 0;
}