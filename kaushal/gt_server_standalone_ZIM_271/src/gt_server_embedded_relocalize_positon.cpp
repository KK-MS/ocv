#include <iostream>
#include <stdio.h>
#include <math.h>

#include "app_struct.h"
#include "packet.h"

using namespace std;

/*
 ** bearing_angle90deg
 **
 ** Road bearing anle rotation by 90 degree to make parallel vehicle bearing angle
 **	to calculate the new Lat & Lon for car Re-localization
 */
float bearing_angle90deg(float roadBearing)
{
	if (roadBearing >= 90)
		roadBearing = roadBearing - 90;
	else
		roadBearing = 360 + roadBearing - 90;

	return roadBearing;
}

/*
 ** bearing_angle180deg
 **
 **	to calculate the new Lat & Lon for car Re-localization
 */
float bearing_angle180deg_TS(float roadBearing_TS)
{
	if (roadBearing_TS >= 180)
		roadBearing_TS = roadBearing_TS - 180;
	else
		roadBearing_TS = roadBearing_TS + 180;

	return roadBearing_TS;
}

/*
 ** LandmarkOdometry
 **
 ** Calculate the New Lat_lon for car position using ODO_D2L
 ** Re-localization of car
 */
int LandmarkOdometry_SL(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);

	double lat0       = ptr_gtMetadata->d8_gt_latitude;    // CRO_lat_nearest
	double lon0       = ptr_gtMetadata->d8_gt_longitude;   // CRO_lon_nearest
	float GT_d2l      = ptr_metadata->f4_odo_distance;     // ODO_D2L
	float roadbearing = ptr_gtMetadata->f4_gt_raodbearing; // GT_road_bearing

	float GT_D2L; // corrected with bearing angle (Diffrence with iNS & road)

	// Bearing angle rotate to lateral Axis to calculate the new Lat & Lon for car Re-localization
	int roadBearing_rot = bearing_angle90deg(roadbearing);

	// Radius north
	double rn = (R * (1 - EN * EN)) / pow((1 - EN * EN * sin(lat0) * sin(lat0)), 1.5);

	// Radius to the east
	double ro = R / (pow(1 - pow(EN, 2) * pow(sin(lat0), 2), 0.5));

	// Scaling factor north in m / ° distance in meters to the north, between latitudes
	double sfn = rn * (M_PI / 180);

	// East scaling factor in m / ° distance in meters to the east, between longitudes
	double sfo = ro * (M_PI / 180) * cos(lat0 * M_PI / 180);

	// 0-360 -> 180 -180
	roadBearing_rot = ((roadBearing_rot + 180) % 360) - 180;

	float y = GT_d2l * cos(roadBearing_rot * M_PI / 180);
	float x = GT_d2l * sin(roadBearing_rot * M_PI / 180);
	
	//Retransformation
	double lat_new = (lat0 + (y / sfn));
	double lon_new = (lon0 + (x / sfo));

	ptr_metadata->d8_out_odo_latitude = lat_new;
	ptr_metadata->d8_out_odo_longitude = lon_new;
	
	printf("\nLat_new_ODO: %.8f, Lon_new_ODO: %.8f\n", lat_new, lon_new);

	return 0;
}

/*
 ** LandmarkOdometry_TS
 **
 ** Calculate the New Lat_lon for car position using ODO_D2TS
 ** Re-localization of car
 */
int LandmarkOdometry_TS(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);

	double lat0       = ptr_gtMetadata->d8_gt_latitude_TS;   // CRO_lat_TS
	double lon0       = ptr_gtMetadata->d8_gt_longitude_TS;  // CRO_lon_TS
	float ODO_D2TS    = ptr_metadata->f4_odo_distance_TS;    // ODO_D2TS
	float bearing_TS  = ptr_gtMetadata->f4_gt_raod_bear2TS;  // GT_raod_bearing2TS

	// Bearing angle rotate to lateral Axis to calculate the new Lat & Lon for car Re-localization
	int bearingAngle_TS_rot = bearing_angle180deg_TS(bearing_TS);

	// Radius north
	double rn = (R * (1 - EN * EN)) / pow((1 - EN * EN * sin(lat0) * sin(lat0)), 1.5);

	// Radius to the east
	double ro = R / (pow(1 - pow(EN, 2) * pow(sin(lat0), 2), 0.5));

	// Scaling factor north in m / ° distance in meters to the north, between latitudes
	double sfn = rn * (M_PI / 180);

	// East scaling factor in m / ° distance in meters to the east, between longitudes
	double sfo = ro * (M_PI / 180) * cos(lat0 * M_PI / 180);

	// 0-360 -> 180 -180
	bearingAngle_TS_rot = ((bearingAngle_TS_rot + 180) % 360) - 180;

	float y = ODO_D2TS * cos(bearingAngle_TS_rot * M_PI / 180);
	float x = ODO_D2TS * sin(bearingAngle_TS_rot * M_PI / 180);

	//Retransformation
	double lat_new_TS = (lat0 + (y / sfn));
	double lon_new_TS = (lon0 + (x / sfo));

	ptr_metadata->d8_out_odo_latitude_TS = lat_new_TS;
	ptr_metadata->d8_out_odo_longitude_TS = lon_new_TS;

	printf("\nLat_new_TS_ODO: %.8f, Lon_new_TS_ODO: %.8f\n", lat_new_TS, lon_new_TS);

	return 0;
}