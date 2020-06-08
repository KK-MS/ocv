#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string>

#include "netrx.h"
#include "packet.h"

using namespace std;

int cal_D2L_bearing(netrx* ptr_server_obj, int roadBearing)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	double lat0 = ptr_gtMetadata->d8_ins_latitude;  // CRO_lat_nearest
	double lon0 = ptr_gtMetadata->d8_ins_longitude; // CRO_lon_nearest
	double lat1 = ptr_metadata->d8_ins_latitude;    // IMU_lat
	double lon1 = ptr_metadata->d8_ins_longitude;   // IMU_lon

	int bearingAngle;

	// Radius north
	double rn = (R * (1 - EN * EN)) / pow((1 - EN * EN * sin(lat0) * sin(lat0)), 1.5);

	// Radius to the east
	double ro = R / (pow(1 - pow(EN, 2) * pow(sin(lat0), 2), 0.5));

	// Scaling factor north in m / ° distance in meters to the north, between latitudes
	double sfn = rn * (M_PI / 180);

	// East scaling factor in m / ° distance in meters to the east, between longitudes
	double sfo = ro * (M_PI / 180) * cos(lat0 * M_PI / 180);

	// Distance between the two GPS points in X - east in meters
	double sx = (lon1 - lon0) * sfo;

	// Distance between the two GPS points in Y - north in meters
	double sy = (lat1 - lat0) * sfn;

	// Distance between the two GPS points
	float GT_d2l = sqrt(pow(sy, 2) + pow(sx, 2));
	
	// Calcualte the INS_bearing angle
	// arctan2 - angle in rad
	float angle_rad = atan2(sx, sy);

	//angle in degree between - 180 / 180
	int angle_deg = angle_rad * (180 / M_PI);

	if ((((angle_deg + 360) % 360) >= 355) || (((angle_deg + 360) % 360) <= 5)) {

		//bearing angle between 0 - 360°
		bearingAngle = (angle_deg + 360);
	}
	else {

		//bearing angle between 0 - 360°
		bearingAngle = (angle_deg + 360) % 360;
	}

	ptr_metadata->u4_ins_bearing = bearingAngle;

	printf("\nBearing_angle: %d\n", bearingAngle);

	// distance line positive distance measure
	if ((bearingAngle >= 355) && (roadBearing <= 5))
	{
		GT_d2l = sin((360 - bearingAngle + roadBearing) * M_PI / 180) * abs(GT_d2l);
	}
	else {

		GT_d2l = sin((bearingAngle - roadBearing) * M_PI / 180) * abs(GT_d2l);
	}

	GT_d2l = GT_d2l * (-1);

	ptr_gtMetadata->f4_gt_distance = GT_d2l;

	printf("\nD2L_corrected: %f m", GT_d2l);

	return 0;
}