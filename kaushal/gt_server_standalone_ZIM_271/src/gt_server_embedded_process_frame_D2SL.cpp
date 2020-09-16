#include <iostream>
#include <stdio.h>

#include "opencv2/opencv.hpp"

#include "app_struct.h"
#include "packet.h"

using namespace std;
using namespace cv;

/*
 ** process_frame
 **
 ** Process the IMU Frame > create the ROI based on GT_D2L
 ** Calcualte the ODO_D2L & Dispaly the Process Frame
 ** variable use frame_SL, IMU_gps_status, GT_D2L for creating the ROI to find thr ODO_D2L
 ** Calculated ODO_D2L pass to the structure to use it for relocalization of car posiotion
 **
 ** Also able to claculate directly from the measurement video to process the ODO_D2L
 */
int process_frame_D2SL(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);
	
	// Parameters for ROI based on GT data
	float GT_D2L;   // in m
	int GTD2L;      // in mm
	int pix_lane;
	int GT_point_lane_x;
	int GT_point_lane_y;
	int ROI_x1;
	int ROI_y1;
	int ROI_x2;
	int ROI_y2;
	int gps_status;
	
	// Variables for getting the Pixel value
	Scalar pixelValue;
	uchar pixel;

	// lane threshold parameters
	int iLanePixelCount = 0;
	int iLanePixelThreshold = 10;
	int iLaneColorUpperThreshold = 250;

	Mat frame, resize_frame, edges, adrive_logo, new_logo;
	Mat whiteLane, LinesImg, HSV_Img, HLS_Img, frame_blur;

	// load the frame to be process(d2l)
	frame = ptr_struct_obj->frame_SL;
	
	if ((ptr_struct_obj->relocalisation_app == RUN_RELOCALISATION_APP)) {

		// Load the calculated GT_D2L to create the ROI to calculate the Odo_D2L
		GT_D2L = ptr_gtMetadata->f4_gt_distance;

		// Load the IMU_GPS status for selecting the ROI
		gps_status = ptr_metadata->u4_ins_gps_status;

		// Convert the GT_D2L in mm
		GTD2L = GT_D2L * 1000;

		// Convert the D2L in to pixels
		pix_lane = GTD2L / ptr_struct_obj->pixel_dist;

		// GT point on Lnae (x,y)
		GT_point_lane_x = ptr_struct_obj->start_X + pix_lane;
		GT_point_lane_y = ptr_struct_obj->start_Y;

		// Select the ROI on Main Frame based on GPS_Status & Calibration Distance (Tyre to lane Maximum Distance in frame)
		if (GT_D2L > 0 && GT_D2L < ptr_struct_obj->cali_max_D2L) {

			if (gps_status == GPS_STATUS_8) {

				// Auto ROI create based on GT_D2L
				//create the points (x1, y1, x2, Y2) fro ROI
				ROI_x1 = GT_point_lane_x - 25;
				ROI_y1 = GT_point_lane_y - 25;

				// TODO: based on the image qualityand processing output.
				// This can change if the processing output confident is not sufficient.
				ROI_x2 = 50;
				ROI_y2 = 50;
			}
			else if (gps_status == GPS_STATUS_4) {

				// Auto ROI create based on GT_D2L
				//create the points (x1, y1, x2, Y2) fro ROI
				ROI_x1 = GT_point_lane_x - 35;
				ROI_y1 = GT_point_lane_y - 35;

				// TODO: based on the image qualityand processing output.
				// This can change if the processing output confident is not sufficient.
				ROI_x2 = 70;
				ROI_y2 = 70;
			}
			else if (gps_status == GPS_STATUS_2) {

				// Auto ROI create based on GT_D2L
				//create the points (x1, y1, x2, Y2) fro ROI
				ROI_x1 = GT_point_lane_x - 45;
				ROI_y1 = GT_point_lane_y - 45;

				// TODO: based on the image qualityand processing output.
				// This can change if the processing output confident is not sufficient.
				ROI_x2 = 90;
				ROI_y2 = 90;
			}
			else if (gps_status == GPS_STATUS_1 && (GT_D2L < 0.86)) {

				// Auto ROI create based on GT_D2L
				//create the points (x1, y1, x2, Y2) fro ROI
				ROI_x1 = GT_point_lane_x - 60;
				ROI_y1 = GT_point_lane_y - 60;

				// TODO: based on the image qualityand processing output.
				// This can change if the processing output confident is not sufficient.
				ROI_x2 = 120;
				ROI_y2 = 120;
			}
			else {
				// Manual ROI create
				ROI_x1 = 560;
				ROI_y1 = 240;
				ROI_x2 = 80;
				ROI_y2 = 100;
			}
		}
		else {
			// Manual ROI create
			ROI_x1 = 560;
			ROI_y1 = 240;
			ROI_x2 = 80;
			ROI_y2 = 100;
		}

		cout << "GT_D2L: " << GTD2L << " mm,";
		printf(" Pix_ROI: %d", pix_lane);

		printf("\nROI: X1 = %d, Y1 = %d, x2 = %d,Y2 = %d", ROI_x1, ROI_y1, ROI_x2, ROI_y2);
	}

	if ((ptr_struct_obj->process_video_D2SL == RUN_VIDEO_PROCESS_D2SL_APP)) {

		// Resize the frame
		cv::resize(frame, frame, Size(1280, 720));

		imshow("input_frame", frame);

		// create the points (x1, y1, x2, Y2) fro ROI
		ROI_x1 = ptr_struct_obj->roi_x1;
		ROI_y1 = ptr_struct_obj->roi_y1;
		ROI_x2 = ptr_struct_obj->roi_x2;
		ROI_y2 = ptr_struct_obj->roi_y2;

		printf("\nROI: X1 = %d, Y1 = %d, x2 = %d,Y2 = %d", ROI_x1, ROI_y1, ROI_x2, ROI_y2);
	}

	// Here we define our region of interest
	Rect const box(ROI_x1, ROI_y1, ROI_x2, ROI_y2);
	
	Mat ROI = frame(box);
	
	if ((ptr_struct_obj->process_video_D2SL == RUN_VIDEO_PROCESS_D2SL_APP)) {

		GaussianBlur(ROI, ROI, Size(13, 13), 0);
	}

	// convert our fame to HSV Space
	cvtColor(ROI, HLS_Img, COLOR_BGR2HLS);

	// white color thresholding
	Scalar whiteMinScalar = Scalar(100, 100, 50);
	Scalar whiteMaxScalar = Scalar(255, 189, 255);

	inRange(ROI, whiteMinScalar, whiteMaxScalar, LinesImg);

	// Edge detection using canny detector
	int minCannyThreshold = 190;
	int maxCannyThreshold = 230;
	Canny(LinesImg, LinesImg, minCannyThreshold, maxCannyThreshold, 3, true);
	
	// Morphological Operation
	Mat k = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	
	morphologyEx(LinesImg, LinesImg, MORPH_CLOSE, k, Point(-1, -1), 1);
	
	// now applying hough transform TO dETECT Lines in our image
	vector<Vec4i> lines;

	HoughLinesP(LinesImg, lines, 1, CV_PI / 180, 5, 0, 10);

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		Scalar greenColor = Scalar(0, 255, 0);
		line(ROI, Point(l[0], l[1]), Point(l[2], l[3]), greenColor, 1.5, 4);
	}
	
	// make the process image blur for filter the noise
	GaussianBlur(frame, frame_blur, Size(5, 5), 0);

	// Main frame with Process ROI > Edge detection for ODO_D2L measurement
	Canny(frame_blur, LinesImg, 250, 255, 3, true);
	
	// process ODO_D2L
	for (int i = ptr_struct_obj->start_X; i < ptr_struct_obj->frame_width; i++)
	{
		pixel = (uchar)LinesImg.at<uchar>(Point(i, ptr_struct_obj->end_Y));

		if (pixel >= iLaneColorUpperThreshold) {
			
			// Find the D2L
			Point2f a(ptr_struct_obj->start_X, ptr_struct_obj->start_Y);
			Point2f b(i, ptr_struct_obj->end_Y);
			
			int result = cv::norm(cv::Mat(b), cv::Mat(a));
			
			float distance = result * ptr_struct_obj->pixel_dist * 0.001;

			printf("\nx:%d, y:%d , val= %u", i, ptr_struct_obj->end_Y, pixel);
			printf("\nWe got the lane, found D2L= %f m\n", distance);

			ptr_metadata->f4_odo_distance = distance;

			// Display the Frames
			line(frame, Point(ptr_struct_obj->start_X, ptr_struct_obj->start_Y), Point(i, ptr_struct_obj->end_Y), Scalar(255, 255, 0), 1, 8);
			
			// Draw the rect of ROI_GT on process frame
			rectangle(frame, Point(ROI_x1, ROI_y1), Point((ROI_x1 + ROI_x2), (ROI_y1 + ROI_y2)), Scalar(0, 255, 255), 1, LINE_8);
			
			if ((ptr_struct_obj->relocalisation_app == RUN_RELOCALISATION_APP)) {

				// put the text Frame number on frame for information
				putText(frame, format(" Frame: %d", ptr_metadata->u4_frame_number), Point(0, 430), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));
			
				// put the text D2L on Real frame for information
				putText(frame, format(" D2L: %f m", distance), Point(0, 460), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));
			
				// put the Frame GPS Status on frame for information
				putText(frame, format(" INS_GPS: %d", gps_status), Point(0, 40), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));

				// put the text Frame number on frame for information
				putText(frame, ptr_struct_obj->road, Point(530, 40), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 255, 255));
				putText(frame, ptr_struct_obj->direction, Point(600, 40), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 255, 255));

				if (ptr_struct_obj->adrive_logo == ADRIVE_LOGO) {

					// Put the LOGO on frame
					ptr_struct_obj->mat_logo_SL.copyTo(frame(cv::Rect(540, 440, ptr_struct_obj->mat_logo_SL.cols, ptr_struct_obj->mat_logo_SL.rows)));
				}
				else {

					// Draw the rect for dispalying the info on process frame
					rectangle(frame, Point(435, 440), Point(620, 467), Scalar(255, 255, 255), -1, LINE_8);

					// Put the company inforamtion on frame
					putText(frame, format("Adrive Living Lab"), Point(440, 460), FONT_ITALIC, 0.65, Scalar(255, 150, 45));
				}
			}

			if ((ptr_struct_obj->process_video_D2SL == RUN_VIDEO_PROCESS_D2SL_APP)) {

				// put the text Frame number on frame for information
				putText(frame, format(" Frame: %d", ptr_metadata->u4_frame_number), Point(20, 70), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));

				// put the text D2L on frame for information
				putText(frame, format(" D2L: %f m", distance), Point(20, 700), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));
				
				if (ptr_struct_obj->adrive_logo == ADRIVE_LOGO) {

					// Put the LOGO on frame
					ptr_struct_obj->mat_logo_SL.copyTo(frame(cv::Rect(1120, 650, ptr_struct_obj->mat_logo_SL.cols, ptr_struct_obj->mat_logo_SL.rows)));
				}
				else {

					// Draw the rect for dispalying the info on process frame
					rectangle(frame, Point(1045, 675), Point(1265 , 708), Scalar(255, 255, 255), -1, LINE_8);

					// Put the company (Adrive LL) inforamtion on frame
					putText(frame, format("Adrive Living Lab"), Point(1050, 700), FONT_ITALIC, 0.8, Scalar(255, 150, 45));
				}
			}

			if (ptr_struct_obj->vo_process_frames_video_save == VO_PROCESS_FRAME_VIDEO_SAVE) {

				// Write to video file
				ptr_struct_obj->wrOutVideo.write(frame);
			}
			
			// Display the process frame
			imshow("Process_frame_D2L", frame);
			
			break;
		}
	}

    return 0;
}