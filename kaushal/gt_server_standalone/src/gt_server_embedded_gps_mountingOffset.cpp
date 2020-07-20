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
int cal_vector_distance_bearing(netrx* ptr_server_obj, double lat_odo_new, double lon_odo_new, double lat_odo_prev, double lon_odo_prev)
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

	// Distance between the two GPS points
	float d2mo = sqrt(pow(sy, 2) + pow(sx, 2));

	ptr_metadata->f4_mo_distance = d2mo;
	
	// Calcualte the INS_bearing angle
	
	// arctan2 - angle in rad
	float angle_rad = atan2(sx, sy);

	//angle in degree between - 180 / 180
	int angle_deg = angle_rad * (180 / M_PI);

	//bearing angle between 0 - 360°
	bearingAngle = (angle_deg + 360) % 360;

	ptr_metadata->f4_mo_bearingAngle = bearingAngle;
	
	return 0;
}

/*
 ** bearing_angle90deg
 **
 ** bearing anle rotation by 90 degree
 */
int bearing_angle90deg(int bearing)
{
	if (bearing >= 90)
		bearing = bearing - 90;
	else
		bearing = 360 + bearing - 90;

	return bearing;
}

/*
 ** gps_offset_correction
 **
 ** Calculate the New Lat_lon for car GPS Mounting position
 */
int gps_offset_correction(netrx*  ptr_server_obj, float offset, int bearing_angle, double lat_odo_new, double lon_odo_new)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);
	
	// Radius north
	double rn = (R * (1 - EN * EN)) / pow((1 - EN * EN * sin(lat_odo_new) * sin(lat_odo_new)), 1.5);

	// Radius to the east
	double ro = R / (pow(1 - pow(EN, 2) * pow(sin(lat_odo_new), 2), 0.5));

	// Scaling factor north in m / ° distance in meters to the north, between latitudes
	double sfn = rn * (M_PI / 180);

	// East scaling factor in m / ° distance in meters to the east, between longitudes
	double sfo = ro * (M_PI / 180) * cos(lat_odo_new * M_PI / 180);

	// 0-360 -> 180 -180
	bearing_angle = ((bearing_angle + 180) % 360) - 180;

	float y = offset * cos(bearing_angle * M_PI / 180);
	float x = offset * sin(bearing_angle * M_PI / 180);

	float GT_d2l_new = sqrt(x * x + y * y);
	
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

	// GPS Mounting Position Offset from Car Tyre (FR)
	float lat_offset  = -0.947;
	float long_offset = 2.314;

	double lat_odo_new = ptr_metadata->d8_out_odo_latitude ;       // lat_ODO_new
	double lon_odo_new = ptr_metadata->d8_out_odo_longitude;       // lon_ODO_new
	
	double lat_odo_prev = ptr_metadata->d8_out_odo_latitude_prev;  // lat_ODO_new_previous
	double lon_odo_prev = ptr_metadata->d8_out_odo_longitude_prev; // lon_ODO_new_previous

	// calulate the Distance and angle from New_odo_Lat_lon to prvious new_odo_lat_lon
	cal_vector_distance_bearing(ptr_server_obj, lat_odo_new, lon_odo_new, lat_odo_prev, lon_odo_prev);

	int bearing_angle = ptr_metadata->f4_mo_bearingAngle;
	
	int bearing_angle_rot = bearing_angle90deg(bearing_angle);

	// logitudnal GPS correction
	gps_offset_correction(ptr_server_obj, long_offset, bearing_angle, lat_odo_new, lon_odo_new);

	// Variables for taking the calculated logitudnal offset position form car tyre (FR)
	double lat_odo_mo_lat = ptr_metadata->d8_mo_ins_latitude_antenna;
	double lon_odo_mo_lat = ptr_metadata->d8_mo_ins_longitude_antenna;

	// Latitudnal GPS correction
	gps_offset_correction(ptr_server_obj, lat_offset, bearing_angle_rot, lat_odo_mo_lat, lon_odo_mo_lat);
	
	// Variables for taking the calculated Lateral offset position > GPS_Mounting_Position form car tyre (FR)
	double lat_new_mo = ptr_metadata->d8_mo_ins_latitude_antenna;
	double lon_new_mo = ptr_metadata->d8_mo_ins_longitude_antenna;

	// Valiation
	cal_vector_distance_bearing(ptr_server_obj, lat_odo_new, lon_odo_new, lat_new_mo, lon_new_mo);

	// Distance from ODO_new position (FR Tyre) to GPS mounting Position > Diagonal
	float d2mo = ptr_metadata->f4_mo_distance;
	
	printf("\nD2GPS_mounting_validate from FR wheel: %f m\n", d2mo);

	// store the current ODO_new position into structure for calcilatig the Mounting offset for next frame
	ptr_metadata->d8_out_odo_latitude_prev  = lat_odo_new;
	ptr_metadata->d8_out_odo_longitude_prev = lon_odo_new;

	return 0;
}