/*****************************************************************************
*
* A Drive Living Lab Confidential Proprietary
*
* Copyright (c) 2019 A Drive Living lab
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "netrx.h"
#include "packet.h"

#include "opencv2/opencv.hpp"

#pragma warning(disable:4996)

#define CM_VDS_FRAME_WIDTH  (640)
#define CM_VDS_FRAME_HEIGHT (480)

#define CM_FRAME_SIZE  (CM_VDS_FRAME_WIDTH * CM_VDS_FRAME_HEIGHT * 3)

using namespace std;

/* Function declaration */
//int process_data(netrx* ptrCliNet, char* ptr_frame_buf, int height, int width);

/*
 ** get_data_stream_process
 **
 ** Getting the CarMaker Streamimg METADATA & FRAMEDATA & processing(D2L) it.
 */
int get_data_stream_process(netrx* ptrCliNet)
{
	int socket_desc;
	int read_size;

	int width = CM_VDS_FRAME_WIDTH;
	int height = CM_VDS_FRAME_HEIGHT;
	int ImgLen = CM_FRAME_SIZE;
	int len = 0;
	int res = 0;

	char* img = (char*)malloc(ImgLen);

	// TODO Rename the sock_desc_gt_bridge to input data

	socket_desc = ptrCliNet->sock_desc_gt_bridge;

	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptrCliNet->stPacket);
	
	//  APO -> NXP_server : CM Data
	read_size = recv(socket_desc, (char*)ptr_metadata, sizeof(PACKET), 0);
	if (read_size < 0) {
		printf("***Error in Receiving MetaData: [%d] : %s\n"
			, errno, strerror(errno));
		return -1;
	}

	if (read_size != sizeof(PACKET)) {
		printf("***Error: RX METADATA PACKET got %d\n", read_size);
		// TODO: log the error and request again for metadata
	}
	printf("read_size:%d\n", read_size);

	// Display the CM_GPS + TS + CM_D2L Data
	printf("\nTimeL: %f\n", ptr_metadata->u4_timestampL);
	printf("lat: %f\n", ptr_metadata->u4_ins_latitude);
	printf("lon: %f\n", ptr_metadata->u4_ins_longitude);
	printf("CM_D2L: %f\n", ptr_metadata->u4_ins_cm_d2l);

#if 0
	//  APO -> NXP_server : CM_VDS_Frame
	for (len = 0; len < ImgLen; len += res) {

		if ((res = recv(socket_desc, img + len, ImgLen - len, 0)) < 0) {
			printf("VDS: Socket Reading Failure\n");
			free(img);
			break;
		}
	}
	printf("\nReceive size: %d\n", len);

	// TODO process function outside of this function
	// Processing the receive CM_VDS Farme (D2L)
	//process_data(ptrCliNet, img, height, width);
#endif
	return 0;
}

/*
 ** get_gt_sidelane_info
 **
 ** Send the Carmaker Metadata and frame Processing (D2L) data to GT_Client for
 ** Correction the car Position(GPS) using GT_METADTA.
 ** Receive the new Car position (GPS) & Side lane GPS from GT_Client.
 */
int get_gt_sidelane_info(netrx* ptrCliNet)
{
	int send_size;
	int read_size;

	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptrCliNet->stPacket);

	// send CM Data to GT_server
	send_size = send(ptrCliNet->sock_desc_gt, (char*)ptr_metadata, sizeof(PACKET), 0);
	if (send_size == -1) {
		printf("***Error in sending Request for MetaData: [%d] : %s\n"
			, errno, strerror(errno));
		return -1;
	}
	printf("send_size:%d\n", send_size);

	// Getting corrected METADATA from GT_Server
	read_size = recv(ptrCliNet->sock_desc_gt, (char *)ptr_metadata, sizeof(PACKET), 0);
	if (read_size < 0) {
		printf("***Error in Receiving MetaData: [%d] : %s\n"
			, errno, strerror(errno));
		return -1;
	}

	if (read_size != sizeof(PACKET)) {
		printf("***Error: RX METADATA PACKET got %d\n", read_size);
		// TODO: log the error and request again for metadata
	}

	printf("read_size:%d\n", read_size);

	// |TODO get the side lane co-ordinates also
	// Display the corrected car Position Co-ordinates
	printf("New_lat: %f\n", ptr_metadata->u4_out_odo_latitude);
	printf("New_lon: %f\n", ptr_metadata->u4_out_odo_longitude);

	// send New_position to Odo_client
	send_size = send(ptrCliNet->sock_desc_odo, (char*)ptr_metadata, sizeof(PACKET), 0);
	if (send_size == -1) {
		printf("***Error in sending Request for MetaData: [%d] : %s\n"
			, errno, strerror(errno));
		return -1;
	}
	printf("send_size:%d\n", send_size);

	return 0;
}

/*
 ** run_app
 **
 ** Receive the CM_METADATA & FRAMEDATA, Processing frame and coorection the Car
 ** position using GT_METADTA and receiving back the new Car Position (GPS) and
 ** Side Lane GPS Values.
 */
int run_app(netrx* ptrCliNet)
{
	int ret = 0;

	//printf("\nRun_App\n");

	// For add the logo to main Frame to display
	const String logo = "./ADrive_Logo1.png";
	Mat mlogo = imread(logo, IMREAD_COLOR);

	// Resize the logo
	cv::resize(mlogo, ptrCliNet->mat_logo, Size(135, 50));

	// For Saving the process frames as video
	//const String name = ptrCliNet->vd_filename;

	//ptrCliNet->wrOutVideo.open(name, CV_FOURCC('M', 'J', 'P', 'G'), 10.0, Size(WIDTH, HEIGHT), true);

	// Receiving the METADATA & FRAMEDATA from CM_APO
	while (1) {

		fflush(stdin);

		if ((ret = get_data_stream_process(ptrCliNet)) < 0) {
			return ret;
		}

		// TODO Process function here

		// car position (GPS) Correction
		if ((ret = get_gt_sidelane_info(ptrCliNet)) < 0) {
			return ret;
		}

	} // loop

	return 0;
}