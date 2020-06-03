#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>

#include "opencv2/opencv.hpp"

#include "netrx1.h"
#include "packet.h"

using namespace std;
using namespace cv;

// 24/04/2020
#define START_X      160      // 160
#define START_Y      272      // 272 // 285
#define END_Y        START_Y  // 380
#define FRAME_WIDTH  640      // 1280 // 640

#define PIXEL_DIST   2.1      // in to mm  by calibration // 2.28

int process_frame(netrx* ptr_server_obj)
{
	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptr_server_obj->stPacket);

	GT_LANE_PACKET* ptr_gtMetadata = (GT_LANE_PACKET*) & (ptr_server_obj->stGtLanePacket);

	int i = 0;
	FILE *file;
 
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

	// Variables for getting the Pixel value
	Scalar pixelValue;
	uchar pixel;

	// lane threshold parameters
	int iLanePixelCount = 0;
	int iLanePixelThreshold = 10;
	int iLaneColorUpperThreshold = 250;

	Mat frame, resize_frame, edges, adrive_logo, new_logo;
	Mat whiteLane, LinesImg, HSV_Img;

	frame = ptr_server_obj->frame;

	GT_D2L = ptr_gtMetadata->f4_gt_distance;  // GT_D2L

	// Convert the GT_D2L in mm
	GTD2L = GT_D2L * 1000;

	// Convert the D2L in to pixels
	pix_lane = GTD2L / PIXEL_DIST;

	// GT point on Lnae (x,y)
	GT_point_lane_x = START_X + pix_lane;
	GT_point_lane_y = START_Y;

#if 0
	// Auto ROI create based on GT_D2L
	//create the points (x1, y1, x2, Y2) fro ROI
	ROI_x1 = GT_point_lane_x - 25;
	ROI_y1 = GT_point_lane_y - 25;
	ROI_x2 = 50;
	ROI_y2 = 50;
#endif

#if 1
	// Manual ROI create
	ROI_x1 = 420;
	ROI_y1 = 240;
	ROI_x2 = 210;
	ROI_y2 = 100;
#endif

	cout << "GT_D2L: " << GTD2L << " mm," ;
	printf(" Pix_ROI: %d", pix_lane);
	
	printf("\nROI: X1= % d, Y1= % d, x2= % d,Y2= % d", ROI_x1, ROI_y1, ROI_x2, ROI_y2);

	// Here we define our region of interest
	Rect const box(ROI_x1, ROI_y1, ROI_x2, ROI_y2);

	Mat ROI = frame(box);

	// convert our fame to HSV Space
	cvtColor(ROI, HSV_Img, COLOR_BGR2HSV);

	// white color thresholding_14.04.20
	Scalar whiteMinScalar = Scalar(100, 100, 50); // 10 0 90 // 100 100 50 //
	Scalar whiteMaxScalar = Scalar(255, 189, 255); // 50 189 255 // 250 189 255

	inRange(ROI, whiteMinScalar, whiteMaxScalar, LinesImg);

	// Edge detection using canny detector
	int minCannyThreshold = 190; // 190
	int maxCannyThreshold = 230; // 230
	Canny(LinesImg, LinesImg, minCannyThreshold, maxCannyThreshold, 3, true);

	// Morphological Operation
	Mat k = getStructuringElement(MORPH_RECT, Size(3, 3));

	morphologyEx(LinesImg, LinesImg, MORPH_CLOSE, k);
	
	// now applying hough transform TO dETECT Lines in our image
	vector<Vec4i> lines;

	HoughLinesP(LinesImg, lines, 1, CV_PI / 180, 5, 0, 10); // process_2

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];                     // we have 4 elements p1=x1,y1  p2= x2,y2
		Scalar greenColor = Scalar(0, 255, 0);  // B=0 G=250 R=30
		line(ROI, Point(l[0], l[1]), Point(l[2], l[3]), greenColor, 1.5, 4);
	}

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

		//printf("x:%d, y:%d , val= %u \n", i, END_Y, pixel);

		if (pixel >= iLaneColorUpperThreshold) {
			
			// Find the D2L
			Point2f a(START_X, START_Y);
			Point2f b(i, END_Y);
			
			int result = cv::norm(cv::Mat(b), cv::Mat(a));
			
			float distance = result * PIXEL_DIST * 0.001; // 0.26 // 0.41 // Procoess_2

			printf("\nx:%d, y:%d , val= %u \n", i, END_Y, pixel);
			printf("We got the lane, found D2L= %f m\n", distance);

			ptr_metadata->f4_odo_distance = distance;

			// Display the Frames
			line(frame, Point(START_X, START_Y), Point(i, END_Y), Scalar(255, 255, 0), 1, 8);
			//line(edges, Point(START_X, END_Y), Point(i, END_Y), Scalar(255, 255, 255), 1, 8);

			// Draw the rect of ROI_GT on process frame
			rectangle(frame, Point(ROI_x1, ROI_y1), Point((ROI_x1 + ROI_x2), (ROI_y1 + ROI_y2)), Scalar(0, 255, 255), 1, LINE_8);
			
			// put the text Frame number on real frame for information
			putText(frame, format("Frame: %s", ptr_server_obj->frame_name), Point(0, 450), FONT_HERSHEY_PLAIN, 0.75, Scalar(255, 255, 255));
			
			// put the text D2L on Real frame for information
			putText(frame, format(" D2L: %f m", distance), Point(0, 420), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));
			
			// Put the LOGO on real frame
			ptr_server_obj->mat_logo.copyTo(frame(cv::Rect(540, 440, ptr_server_obj->mat_logo.cols, ptr_server_obj->mat_logo.rows)));
			
			// Write to video file
			ptr_server_obj->wrOutVideo.write(frame);
			
			// Display the original frame or process frame
			imshow("Process_frame_D2L", frame);

			break;
		}
	}
			
  return 0;
}