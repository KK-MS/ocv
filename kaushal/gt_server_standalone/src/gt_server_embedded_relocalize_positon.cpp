#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string>

#include "netrx1.h"
#include "packet.h"

using namespace std;

int bearing_angle90deg(netrx* ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	int bearing = ptr_metadata->u4_ins_bearing;

	if (bearing >= 90)
		bearing = bearing - 90;
	else
		bearing = 360 + bearing - 90;

	return bearing;
}

int LandmarkOdometry(netrx* ptr_server_obj, int bearingAngle_rot, int roadBearing)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	double lat0 = ptr_gtMetadata->d8_ins_latitude;  // CRO_lat_nearest
	double lon0 = ptr_gtMetadata->d8_ins_longitude; // CRO_lon_nearest
	float GT_d2l = ptr_metadata->f4_odo_distance;   // ODO_D2L
	int ins_bearing = ptr_metadata->u4_ins_bearing; // INS_Bearing_angle

	float GT_D2L; // corrected with bearing angle (Diffrence with iNS & road)

	// Radius north
	double rn = (R * (1 - EN * EN)) / pow((1 - EN * EN * sin(lat0) * sin(lat0)), 1.5);

	// Radius to the east
	double ro = R / (pow(1 - pow(EN, 2) * pow(sin(lat0), 2), 0.5));

	// Scaling factor north in m / ° distance in meters to the north, between latitudes
	double sfn = rn * (M_PI / 180);

	// East scaling factor in m / ° distance in meters to the east, between longitudes
	double sfo = ro * (M_PI / 180) * cos(lat0 * M_PI / 180);

	// 0-360 -> 180 -180
	bearingAngle_rot = ((bearingAngle_rot + 180) % 360) - 180;

	float y = GT_d2l * cos(bearingAngle_rot * M_PI / 180);
	float x = GT_d2l * sin(bearingAngle_rot * M_PI / 180);

	float GT_d2l_new = sqrt(x * x + y * y);

	//printf("\n\nBearingAngle_rot: %d, Y: %f, X: %f, GT_D2L: %f", bearingAngle_rot, y, x, GT_d2l_new);

	//Retransformation
	double lat_new = (lat0 + (y / sfn));
	double lon_new = (lon0 + (x / sfo));

	ptr_gtMetadata->d8_gt_latitude = lat_new;
	ptr_gtMetadata->d8_gt_longitude = lon_new;

	printf("\nLat_new: %.8f, Lon_new: %.8f\n", lat_new, lon_new);

	// distance line positive distance measure
	if ((ins_bearing >= 355) && (roadBearing <= 5))
	{
		GT_D2L = sin((360 - ins_bearing + roadBearing) * M_PI / 180) * abs(GT_d2l);
	}
	else {

		GT_D2L = sin((ins_bearing - roadBearing) * M_PI / 180) * abs(GT_d2l);
	}

	printf("\nD2L_corrected: %f m", GT_D2L);

	return 0;
}