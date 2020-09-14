#include <iostream>
#include <stdio.h>

#include "opencv2/opencv.hpp"

#include "app_struct.h"
#include "packet.h"

using namespace std;
using namespace cv;

/*
 ** process_frame_D2TS
 **
 ** Process the IMU Frame > create the ROI based on GT_D2L
 ** Calcualte the ODO_D2L & Dispaly the Process Frame
 */
int process_frame_D2TS(app_struct* ptr_struct_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_struct_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_struct_obj->stGtLanePacket);
	
	// Parameters for ROI based on GT data
	float GT_D2TS;   // in m
	int ROI_x1;
	int ROI_y1;
	int ROI_x2;
	int ROI_y2;
	int gps_status;
		
	int framenumber_TS;
	float width_TS;

	Mat frame_TS, resize_frame, edges, adrive_logo, new_logo;
	Mat ROI_gray, ROI_canny;

	// load the frame to be process(d2l)
	frame_TS = ptr_struct_obj->frame_TS;

	// load the TS width
	width_TS = ptr_gtMetadata->f4_gt_width_TS;

	// load the TS_farme number
	framenumber_TS = ptr_metadata->u4_frame_number_TS;
	
	// Load the calculated GT_D2L to create the ROI to calculate the Odo_D2L
	GT_D2TS = ptr_gtMetadata->f4_gt_distance_TS;

	// TS_Frame resize
	cv::resize(frame_TS, frame_TS, Size(1280, 720));
		
	// Manual ROI create
	ROI_x1 = 750;
	ROI_y1 = 250;
	ROI_x2 = 500;
	ROI_y2 = 250;
	
	printf("\nROI: X1 = %d, Y1 = %d, x2 = %d,Y2 = %d", ROI_x1, ROI_y1, ROI_x2, ROI_y2);

	// Here we define our region of interest
	Rect const box(ROI_x1, ROI_y1, ROI_x2, ROI_y2);

	Mat ROI = frame_TS(box);

	// Convert the ROI from color to gray scale
	cvtColor(ROI, ROI_gray, COLOR_BGR2GRAY);

	//Canny(src_gray, canny_ROI, 200, 240, 3, true);
	Canny(ROI_gray, ROI_canny, 80, 240); // , 3, true);

	imshow("canny_ROI", ROI_canny);

	vector<vector<Point> > contours;
	//vector<Vec4i> hierarchy;
	
	// Find contours
	//findContours(ROI_canny, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//findContours(ROI_canny.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	findContours(ROI_canny.clone(), contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<vector<Point> > contours_poly(contours.size());
	cv::Rect bounding_rect;

	for (size_t i = 0; i < contours.size(); i++)
	{

		//printf("\n [%d]", i);
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

		// Skip small or non-convex objects
		if (std::fabs(cv::contourArea(contours[i])) < 430 || !cv::isContourConvex(contours_poly[i]))
			continue;

		int largest_area = 0;
		int largest_contour_index = 0;

		double area = contourArea(contours[i], false);  // Find the area of contour
		if (area > largest_area) {
			largest_area = area;
			largest_contour_index = i;                  // Store the index of largest contour
			bounding_rect = boundingRect(contours[i]);  // Find the bounding rectangle for biggest contour

			float distance = (width_TS * 754) / bounding_rect.width;

			ptr_metadata->f4_odo_distance_TS = distance;

			printf("\nlarge_area: %d", largest_area);
			printf("\nwidth: %d", bounding_rect.width);
			printf("\nD2TS: %f m\n", distance);
		
			putText(frame_TS, format(" D2TS: %f m", distance), Point(50, 670), FONT_HERSHEY_PLAIN, 2, Scalar(0, 255, 255), 2);

			break;
		}
	}

	// Draw the rectangle (ROI) on Frame_TS
	rectangle(ROI, bounding_rect, Scalar(25, 205, 255), 1, 8, 0);

	// put the txt on frame for information
	putText(frame_TS, format("Frame: %d", ptr_metadata->u4_frame_number_TS), Point(50, 50), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 255), 1.5);

	// Put the LOGO on resize frame
	ptr_struct_obj->mat_logo_TS.copyTo(frame_TS(cv::Rect(1120, 650, ptr_struct_obj->mat_logo_TS.cols, ptr_struct_obj->mat_logo_TS.rows)));

	// Write to video file
	//wrOutVideo.write(frame);

	imshow("D2TS", frame_TS);

	ptr_struct_obj->frame_TS = frame_TS;

    return 0;
}

#if 0

// convert our fame to HSV Space
cvtColor(ROI, HSV_Img, COLOR_BGR2HSV);

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

// Main frame with Process ROI > Edge detection for ODO_D2L measurement
Canny(frame, LinesImg, 250, 255, 3, true);

// process ODO_D2L
for (int i = START_X; i < FRAME_WIDTH; i++)
{
	pixel = (uchar)LinesImg.at<uchar>(Point(i, END_Y));

	if (pixel >= iLaneColorUpperThreshold) {
		iLanePixelCount++;
	}
	else {
		iLanePixelCount = 0;
	}

	if (pixel >= iLaneColorUpperThreshold) {

		// Find the D2L
		Point2f a(START_X, START_Y);
		Point2f b(i, END_Y);

		int result = cv::norm(cv::Mat(b), cv::Mat(a));

		float distance = result * PIXEL_DIST * 0.001;

		printf("\nx:%d, y:%d , val= %u", i, END_Y, pixel);
		printf("\nWe got the lane, found D2L= %f m\n", distance);

		ptr_metadata->f4_odo_distance = distance;

		// Display the Frames
		line(frame, Point(START_X, START_Y), Point(i, END_Y), Scalar(255, 255, 0), 1, 8);

		// Draw the rect of ROI_GT on process frame
		rectangle(frame, Point(ROI_x1, ROI_y1), Point((ROI_x1 + ROI_x2), (ROI_y1 + ROI_y2)), Scalar(0, 255, 255), 1, LINE_8);

		// put the text Frame number on real frame for information
		putText(frame, format(" Frame: %d", ptr_metadata->u4_frame_number), Point(0, 450), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));

		// put the text D2L on Real frame for information
		putText(frame, format(" D2L: %f m", distance), Point(0, 420), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));

		// put the Frame GPS Status on Real frame for information
		putText(frame, format(" INS_GPS: %d", gps_status), Point(0, 40), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));

		// put the text Frame number on real frame for information
		putText(frame, ptr_server_obj->road, Point(560, 40), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 255, 255));
		putText(frame, ptr_server_obj->direction, Point(600, 40), FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 255, 255));

		// Put the LOGO on real frame
		ptr_server_obj->mat_logo.copyTo(frame(cv::Rect(540, 440, ptr_server_obj->mat_logo.cols, ptr_server_obj->mat_logo.rows)));

		if (PROCESS_FRAMES_VIDEO_SAVE == 1) {

			// Write to video file
			ptr_server_obj->wrOutVideo.write(frame);
		}

		// Display the original frame or process frame
		imshow("Process_frame_D2L", frame);

		break;
	}
}
#endif