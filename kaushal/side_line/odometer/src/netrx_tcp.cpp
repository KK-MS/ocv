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

using namespace std;
using namespace cv;

/* Function declaration */
int process_data(netrx* ptrCliNet, char* ptr_frame_buf, int height, int width);

/*
 ** get_data_stream_process
 **
 ** Getting the CarMaker Streamimg METADATA & FRAMEDATA & processing(D2L) it.
 */
int get_data_stream(netrx* ptrCliNet)
{
	int socket_desc;
	int read_size;

	int len = 0;
	int res = 0;

    socket_desc = ptrCliNet->sock_desc_input_data;

	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptrCliNet->stPacket);

	//  APO -> Odometer : CM Data
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

	//  APO -> Odometer : CM_VDS_Frame
	for (len = 0; len < ptrCliNet->ImgLen; len += res) {

		res = recv(socket_desc, (char *)ptrCliNet->img, ptrCliNet->ImgLen, 0);
		if (res  < 0)
		{
			printf("VDS: Socket Reading Failure\n");
			free(ptrCliNet->img);
			break;
		}
	}
	printf("\nReceive size: %d\n", len);

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

	// Display the corrected car Position Co-ordinates
	printf("New_lat: %f\n", ptr_metadata->u4_out_odo_latitude);
	printf("New_lon: %f\n", ptr_metadata->u4_out_odo_longitude);

	return 0;
}

/*
 ** application_info
 **
 ** Send the new car position with Updated METADATA Packet to application.
 ** For the Analysis or use in other applications.
 */
int application_info(netrx* ptrCliNet)
{
	int send_size;
	int read_size;

	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptrCliNet->stPacket);

	// Send car new_position to Application
	send_size = send(ptrCliNet->sock_desc_app, (char*)ptr_metadata, sizeof(PACKET), 0);
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
		
	// Receiving the METADATA & FRAMEDATA from CM_APO
	while (1) {

		fflush(stdin);

		// Step-1: It receives the input data (IMU + Images),
		// Get IMU + Image data,
		if ((ret = get_data_stream(ptrCliNet)) < 0) {
			return ret;
		}

#if 1
		// Step-2: Odometry pass IMU data to Ground truth and get the Sidelane/Traffic sign details.
		// car position (GPS) Correction
		if ((ret = get_gt_sidelane_info(ptrCliNet)) < 0) {
			return ret;
		}
#endif

		// Step-3: Odometry calculates the Region of Interst from IMU Lat-Long-bearing and GT Lat-long
		// currently, It is done in step - 4 

#if 1
		// Step-4: Odometry calculates teh distance of the intersted object/lane
		// Processing the receive CM_VDS Farme (D2L)
		if ((ret = process_data(ptrCliNet, ptrCliNet->img, ptrCliNet->height, ptrCliNet->width)) < 0) {
			return ret;
		}
				
		//free(ptrCliNet->img);
		//free(frame_buf);
#endif

		// Step-5: Odometry calculates the current position from Distane and GT Lat-long.
		// Currently, it is already done in step - 2 GT_client side. 
		// Later stage will do in this step host side.

#if 1
		// Step-6: Odometry passt the calculated value to the application.
		if ((ret = application_info(ptrCliNet)) < 0) {
			return ret;
		}
#endif

	} // loop

	return 0;
}
