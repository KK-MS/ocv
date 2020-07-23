#include <iostream>
#include <stdio.h>
#include <math.h>

#include "netrx.h"
#include "packet.h"

using namespace std;

/*
 ** cal_vector_distance_bearing
 **
 ** Calulate the Distance and angle from New_odo_Lat_lon to prvious new_odo_lat_lon
 */
int cal_vector_bearing(netrx* ptr_server_obj, double lat_odo_new, double lon_odo_new, double lat_odo_prev, double lon_odo_prev)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	int bearingAngle;

	// Radius north
	double rn = (R * (1 - EN * EN)) / pow((1 - EN * EN * sin(lat_odo_new) * sin(lat_odo_new)), 1.5);

	// Radius to the east
	double ro = R / (pow(1 - pow(EN, 2) * pow(sin(lat_odo_new), 2), 0.5));

	// Scaling factor north in m / ° distance in meters to the north, between latitudes
	double sfn = rn * (M_PI / 180);

	// East scaling factor in m / ° distance in meters to the east, between longitudes
	double sfo = ro * (M_PI / 180) * cos(lat_odo_new * M_PI / 180);

	// Distance between the two GPS points in X - east in meters
	double sx = (lon_odo_prev - lon_odo_new) * sfo;

	// Distance between the two GPS points in Y - north in meters
	double sy = (lat_odo_prev - lat_odo_new) * sfn;
	
	// Calcualte the INS_bearing angle
	// arctan2 - angle in rad
	float angle_rad = atan2(sx, sy);

	//angle in degree between - 180 / 180
	int angle_deg = angle_rad * (180 / M_PI);

	//bearing angle between 0 - 360°
	bearingAngle = (angle_deg + 360) % 360;

	// Pass the calculated bearing angle in to structure
	ptr_metadata->f4_mo_bearingAngle = bearingAngle;
	
	return 0;
}

/*
 ** gps_offset_correction
 **
 ** Calculate the New Lat_lon for car GPS Mounting position
 */
int gps_offset_correction(netrx*  ptr_server_obj, float offset, int phi_mo, double lat_odo_new, double lon_odo_new)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);
	
	// Radius North
	double rn = (R * (1 - EN * EN)) / pow((1 - EN * EN * sin(lat_odo_new) * sin(lat_odo_new)), 1.5);

	// Radius to the East
	double ro = R / (pow(1 - pow(EN, 2) * pow(sin(lat_odo_new), 2), 0.5));

	// Scaling factor north in m / ° distance in meters to the north, between latitudes
	double sfn = rn * (M_PI / 180);

	// East scaling factor in m / ° distance in meters to the east, between longitudes
	double sfo = ro * (M_PI / 180) * cos(lat_odo_new * M_PI / 180);

	printf("\nsfn: %.8f, sfo: %.8f\n", sfn, sfo);

	// 0-360 -> 180 -180
	phi_mo = ((phi_mo + 180) % 360) - 180;

	float y = offset * cos(phi_mo * M_PI / 180);
	float x = offset * sin(phi_mo * M_PI / 180);

	//Retransformation
	double lat_new_mo = (lat_odo_new + (y / sfn));
	double lon_new_mo = (lon_odo_new + (x / sfo));

	ptr_metadata->d8_mo_ins_latitude_antenna = lat_new_mo;
	ptr_metadata->d8_mo_ins_longitude_antenna = lon_new_mo;

	printf("\nLat_mo: %.8f, Lon_mo: %.8f\n", lat_new_mo, lon_new_mo);

	return 0;
}

/*
 ** gps_mounting_offset
 **
 ** Calculate the New Lat_lon(GPS Mounting Offset) for car GPS Mounting position
 */
int gps_mounting_offset(netrx* ptr_server_obj)
{
	printf("\nThe Car Relocalization to GPS Mounting (Mounting Offset) ...\n");

	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	// GPS Mounting Position Offset from Car Tyre (FR) > Command line Argument
	float lat_offset = ptr_server_obj->gps_lat_offset;
	float lon_offset = ptr_server_obj->gps_lon_offset;

	// Distance to GPS Mounting from FR Wheel
	float D2mo_FR = sqrt(pow(lat_offset, 2) + pow(lon_offset, 2));

	double lat_odo_new = ptr_metadata->d8_out_odo_latitude ;       // lat_ODO_new
	double lon_odo_new = ptr_metadata->d8_out_odo_longitude;       // lon_ODO_new

	double lat_odo_prev = ptr_metadata->d8_out_odo_latitude_prev;  // lat_ODO_new_previous
	double lon_odo_prev = ptr_metadata->d8_out_odo_longitude_prev; // lon_ODO_new_previous

	// calulate the Distance and angle from New_odo_Lat_lon to prvious new_odo_lat_lon
	cal_vector_bearing(ptr_server_obj, lat_odo_new, lon_odo_new, lat_odo_prev, lon_odo_prev);

	int bearing_angle = ptr_metadata->f4_mo_bearingAngle;
	
	// GPS Mounting  position Angle calculation with refrence to vehicle bearing angle
	int alpha = (acos(lon_offset / D2mo_FR)) * (180 / M_PI);
	
	// Total Bearing angle to GPS mounting position  from ODO_Reloclize Position
	int phi_mo = alpha + bearing_angle;
	
	// GPS correction from ODO_New (FR Wheel) to GPS mounting Position
	gps_offset_correction(ptr_server_obj, D2mo_FR, phi_mo, lat_odo_new, lon_odo_new);

	// store the current ODO_new position into structure for calcilatig the Mounting offset for next frame
	ptr_metadata->d8_out_odo_latitude_prev  = lat_odo_new;
	ptr_metadata->d8_out_odo_longitude_prev = lon_odo_new;
	
	return 0;
}