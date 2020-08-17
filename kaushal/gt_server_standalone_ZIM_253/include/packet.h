/*****************************************************************************
*
* A Drive Living Lab Confidential Proprietary
*
* Copyright (c) 2019 A Drive Living Lab

* All Rights Reserved
*
*****************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY A DRIVE LIVING LAB "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL A DRIVE LIVING LAB OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************/

#ifndef PACKET_H
#define PACKET_H

#include <fstream>

// REQUEST COMMAND FORMAT
// 0xDDCCBBAA
// DD and CC are not used
// BB is SERVER ID
// AA is REQUEST ID
// |--------------------------------------------|
// | SERVER ID | Module name | Description      |
// |--------------------------------------------|
// |  0x01     | VD Server   | Metdata + Frames |
// |  0x02     | GT Server   | Ground truth     |
// |  0x03     | GN Server   | Genesys          |
// |--------------------------------------------|
//
// <ACTION> <MODULE ID> <COMMAND>

enum request_type {
  // VD SERVER
  VD_SERVER_ID   = 0x0100,
  REQ_VD_METADATA,
  REQ_VD_FRAME,

  // GT SERVER
  GT_SERVER_ID   = 0x0200,
  REQ_GT_LANE_INFO,
  REQ_GT_TRAFFIC_SIGN_INFO,
};

// Main request packet
typedef struct packet {
	
	// Request type
	unsigned int  u4_request_type;     //< Request type: METADATA, FRAMEDATA ,LAN INFO

	// Genesys
	unsigned int  u4_timestampL;       //< Timestamp Low bytes to sync the data
	unsigned int  u4_timestampH;       //< Timestamp High bytes to sync the data
	float         f4_ins_confidence;   //< INS GPS coordinates confidence/Satellite
	float         f4_ins_cm_d2l;       //< Reserved for future
	double        d8_ins_latitude;     //< INS latitude
	double        d8_ins_longitude;    //< INS longitude
	float         f4_ins_elevation;    //< INS elevation [Optional]
	float         f4_ins_bearing;      //< Diving bearing angle, Yaw

	// Atlatec
	unsigned int  u4_frame_number;     //< Sequence of frame number
	unsigned int  u4_frame_enc_size;   //< encoded frame size.
	unsigned int  u4_frame_width;      //< Raw image width
	unsigned int  u4_frame_height;     //< Raw image height
	unsigned int  u4_frame_resolution; //< Image resolution
	unsigned int  u4_frame_type;       //< JPEG, etc.
	//unsigned int  u4_frame_reserved2;  //< Reserved
	unsigned int  u4_ins_gps_status;   //< Reserved
	
	// TBD Atlatec other info
	// TBD Ground truth. Lane info, landmark info

	// Odometry
	float  f4_odo_distance;            //< Odometry calculated distance
	float  f4_odo_angle;               //< Relative object angle
	float  f4_odo_reserved1;           //< Reserved
	float  f4_odo_reserved2;           //< Reserved

	// Odometry out parameters
	double d8_out_odo_latitude;        //< Odometry calculated latitude
	double d8_out_odo_longitude;       //< Odometry calculated longitude
	double d8_out_odo_latitude_prev;   //< Odometry calculated latitude
	double d8_out_odo_longitude_prev;  //< Odometry calculated longitude
	float  f4_out_odo_confidence;      //< Odometry calculated confidence
	
	// GPS Mounting Offset Correction Parameters
	float  f4_mo_bearingAngle;            //< Bearing angle to the road
	double d8_mo_ins_latitude_antenna;   //< MOunting Offset calculated INS_latitude
	double d8_mo_ins_longitude_antenna;  //< Mounting Offset calculated INS_longitude
	
} PACKET;

typedef struct ADMA_packet {
	
	unsigned int frame_number;
	
	unsigned int State0;
	unsigned int State1;
	unsigned int State2;
	unsigned int State3;
	
	unsigned int Error0;
	unsigned int Error1;
	unsigned int Error2;
	unsigned int Error3;
	
	float        Rate_Body_X;
	float        Rate_Body_Y;
	float        Rate_Body_Z;
	
	float        Acc_Body_X;
	float        Acc_Body_Y;
	float        Acc_Body_Z;
	
	double       GPS_Lat_Abs;
	double       GPS_Lon_Abs;
	
	float        GPS_Stddev_Lat;
	float        GPS_Stddev_Lon;
	float        GPS_Stddev_Height;
	
	float        GPS_Vel_Frame_X;
	float        GPS_Vel_Frame_Y;
	float        GPS_Vel_Frame_Z;
	
	float        GPS_Vel_Latency;
	float        GPS_Stddev_Vel_X;
	float        GPS_Stddev_Vel_Y;	
	float        GPS_Stddev_Vel_Z;
	
	unsigned int GPS_Time_msec;
	unsigned int GPS_Time_Week;
	unsigned int GPS_Sats_Visible;
	unsigned int GPS_COG;
	
	float        GPS_Height;
	
	unsigned int INS_Time_msec;
	unsigned int Status_GPS_Mode;
	
	double       INS_Lat_Abs;
	double       INS_Lon_Abs;

	float        GT_D2L;
	double       GT_Lat;
	double       GT_Lon;
	float        GT_bear;
	double       GT_ele;

}ADMA_PACKET;

// GT LANE Response packet
typedef struct gt_lane_packet{

	// Response type
	unsigned int u4_response_type;

	// Genesys
	unsigned long u8_timestamp;      //< Timestamp to sync the data
	double        d8_ins_latitude;   //< INS latitude
	double        d8_ins_longitude;  //< INS longitude
	double        d8_gt_latitude;    //< GT latitude
	double        d8_gt_longitude;   //< GT longitude
	float         f4_gt_distance;    //< Distance between INX and GT coordinates

} GT_LANE_PACKET;

// GT LANE CRO Array of Structure Packet
typedef struct gt_lane_cro_packet{

	double        d8_cro_latitude;   //< CRO latitude
	double        d8_cro_longitude;  //< CRO longitude
	
} GT_LANE_CRO_PACKET;

#endif /* PACKET_H */