#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>

#pragma warning(disable:4996)

#include "app_struct.h"
#include "packet.h"

using namespace std;
using namespace cv;

/*
 ** odometry_data_save
 **
 ** Saving the Process odometry data
 */
int odometry_data_save(app_struct *ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);

	// Variable initialization from structure
	FILE* ODO_file      = ptr_struct_obj->odometry_file;       // file for Update the Relocalize Data
	unsigned int time   = ptr_metadata->u4_timestampL;         // IMU_time
	float GT_D2L        = ptr_gtMetadata->f4_gt_distance;      // GT_D2L
	float ODO_D2L       = ptr_metadata->f4_odo_distance;       // ODO_D2L
	float confidence_SL = ptr_metadata->f4_out_odo_confidence; // confidence = GT_D2L - ODO_D2L;
	float confidence_TS = ptr_metadata->f4_out_odo_confidence; // confidence_TS = GT_D2TS - ODO_D2TS;
	string shape_TS = ptr_struct_obj->shape_TS;                // shape of TS

	if (shape_TS == "None")
	{

		fprintf(ODO_file, "%d,%0.8f,%0.8f,%f,%d,%d,%0.8f,%0.8f,%0.8f,%0.8f,%f,,,,,,,,\n"
			, ptr_metadata->u4_frame_number, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude, ODO_D2L
			, FRAME_STATUS_8, ptr_metadata->u4_ins_gps_status, ptr_metadata->d8_out_odo_latitude, ptr_metadata->d8_out_odo_longitude
			, ptr_metadata->d8_mo_ins_latitude_antenna, ptr_metadata->d8_mo_ins_longitude_antenna, confidence_SL);
	}
	else {

		fprintf(ODO_file, "%d,%0.8f,%0.8f,%f,%d,%d,%0.8f,%0.8f,%0.8f,%0.8f,%f,%d,%s,%0.8f,%0.8f,%f,%0.8f,%0.8f,%f,%0.8f,%0.8f\n"
			, ptr_metadata->u4_frame_number, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude, ODO_D2L
			, FRAME_STATUS_8, ptr_metadata->u4_ins_gps_status, ptr_metadata->d8_out_odo_latitude, ptr_metadata->d8_out_odo_longitude
			, ptr_metadata->d8_mo_ins_latitude_antenna, ptr_metadata->d8_mo_ins_longitude_antenna, confidence_SL
			, ptr_metadata->u4_frame_number_TS, ptr_struct_obj->TS_shape, ptr_gtMetadata->d8_gt_latitude_TS, ptr_gtMetadata->d8_gt_longitude_TS
			, ptr_metadata->f4_odo_distance_TS, ptr_metadata->d8_out_odo_latitude_TS, ptr_metadata->d8_out_odo_longitude_TS, confidence_TS
			, ptr_metadata->d8_mo_ins_latitude_antenna, ptr_metadata->d8_mo_ins_longitude_antenna);
	}

#if 0
	if ((ODO_D2L < ptr_struct_obj->cali_max_D2L)) {
		
		// print the data in odometry.csv for analysis or future use
		fprintf(ODO_file, "%d,%.8f,%.8f,%.8f,%.8f,%f,%d,%f,%.8f,%.8f,%d,%d,%f,%.8f,%.8f\n"
			, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			, ptr_gtMetadata->d8_gt_latitude, ptr_gtMetadata->d8_gt_longitude
			, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			, ptr_metadata->f4_odo_distance, ptr_metadata->d8_out_odo_latitude
			, ptr_metadata->d8_out_odo_longitude, ptr_metadata->u4_ins_gps_status
			, FRAME_STATUS_8, confidence, ptr_metadata->d8_mo_ins_latitude_antenna
			, ptr_metadata->d8_mo_ins_longitude_antenna);
	}
	else if ((ODO_D2L > ptr_struct_obj->cali_max_D2L) || (ODO_D2L <= 0)) {

		// print the data in odometry.csv for analysis or future use
		fprintf(ODO_file, "%d,%.8f,%.8f,%.8f,%.8f,%f,%d,%f,%.8f,%.8f,%d,%d,%f,%.8f,%.8f\n"
			, time, ptr_metadata->d8_ins_latitude, ptr_metadata->d8_ins_longitude
			, ptr_gtMetadata->d8_gt_latitude, ptr_gtMetadata->d8_gt_longitude
			, ptr_gtMetadata->f4_gt_distance, ptr_metadata->u4_frame_number
			, ptr_metadata->f4_odo_distance, ptr_metadata->d8_out_odo_latitude
			, ptr_metadata->d8_out_odo_longitude, ptr_metadata->u4_ins_gps_status
			, FRAME_STATUS_0, confidence, ptr_metadata->d8_mo_ins_latitude_antenna
			, ptr_metadata->d8_mo_ins_longitude_antenna);
	}
#endif
	return 0;
}