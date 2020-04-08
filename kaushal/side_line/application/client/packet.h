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
*
****************************************************************************/

#ifndef PACKET_H
#define PACKET_H

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
	unsigned int u4_request_type;      //< Request type: METADATA, FRAMEDATA ,LAN INFO

	// Genesys
	double        u4_timestampL;       //< Timestamp Low bytes to sync the data
	unsigned int  u4_timestampH;       //< Timestamp High bytes to sync the data
	float         u4_ins_confidence;   //< INS GPS coordinates confidence/Satellite
	//float         u4_ins_reserved1;    //< Reserved for future
	//loat         u4_ins_TSign_ds;     //< Reserved for future
	float         u4_ins_cm_d2l;     //< Reserved for future
	float         u4_ins_latitude;     //< INS latitude
	float         u4_ins_longitude;    //< INS longitude
	float         u4_ins_elevation;    //< INS elevation [Optional]
	float         u4_ins_bearing;      //< Diving bearing angle, Yaw

	// Atlatec
	unsigned int  u4_frame_number;     //< Sequence of frame number
	unsigned int  u4_frame_enc_size;   //< encoded frame size.
	unsigned int  u4_frame_width;      //< Raw image width
	unsigned int  u4_frame_height;     //< Raw image height
	unsigned int  u4_frame_resolution; //< Image resolution
	unsigned int  u4_frame_type;       //< JPEG, etc.
	unsigned int  u4_frame_reserved2;  //< Reserved
    //unsigned int  u4_frame_cm_range;   //< Reserved
	
	// TBD Atlatec other info
	// TBD Ground truth. Lane info, landmark info

	// Odometry
	//float  u4_odo_vhcl_yaw;  //< Odometry calculated distance
	float  u4_odo_distance;  //< Odometry calculated distance
	float  u4_odo_angle;     //< Relative object angle
	float  u4_odo_reserved1; //< Reserved
	float  u4_odo_reserved2; //< Reserved

	// Odometry out parameters
	float u4_out_odo_latitude;   //< Odometry calculated latitude
	float u4_out_odo_longitude;  //< Odometry calculated longitude
    float u4_out_odo_confidence; //< Odometry calculated confidence

} PACKET;

// GT LANE Response packet
typedef struct gt_lane_packet{

	// Response type
	unsigned int u4_response_type;

	// Genesys
	unsigned long u8_timestamp;      //< Timestamp to sync the data
	unsigned int  u4_ins_latitude;   //< INS latitude
	unsigned int  u4_ins_longitude;  //< INS longitude

	unsigned int  u4_gt_latitude;   //< GT latitude
	unsigned int  u4_gt_longitude;  //< GT longitude

	unsigned int  u4_gt_distance;   //< Distance between INX and GT coordinates

} GT_LANE_PACKET;

#endif /* PACKET_H */

