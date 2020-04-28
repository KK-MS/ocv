#include <iostream>
#include <fstream>

#pragma warning(disable:4996)

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

#define START_X 243		 // 485 // 885 // 243
#define START_Y 260		 // 380 // 260
#define END_Y  START_Y	 // 380
#define FRAME_WIDTH 640  // 1280 // 640

#define PIXEL_DIST 2.27  // in to mm  by calibration // 2.28 

int main(int argc, char *argv[])
{
  int iFrameCount = 0;
  int i = 0;
  FILE *file;
  
  // Parameters for ROI based on GT data 
  int GT_D2L;
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

  // Load the image files from folder in sequence
  vector<cv::String> fn;
  
  //glob("/home/nxp/kaushal/s32/project/VisionSDK_S32V2_RTM_1_3_0/s32v234_sdk/demos/net/netrx/frames/SL_30s/frame_599.jpg", fn, false);
  
  //glob("D:/A7_measurement_14.4.20/cal/*.png", fn, false); // calabration
  glob("D:/A7_measurement_14.4.20/ZIM212/*.png", fn, false); // ZIM 212

  //glob("D:/A7_measurement_14.4.20/img/*.png", fn, false);
  //glob("D:/A7_measurement_14.4.20/img_process/*.png", fn, false);
  //glob("D:/A7_measurement_14.4.20/ZIM212/*.png", fn, false); // ZIM 212

  // to save the frames as video
  VideoWriter wrOutVideo;
  const String name = "./A7_south_processing.avi";
  //wrOutVideo.open(name, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, Size(1280, 720), true);
  wrOutVideo.open(name, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, Size(640, 480), true);

  // Process the frames to find D2L and save the processed frames in device
  while(1)
  {
  
    // open the file
	file = fopen("A7_south_process.csv", "a");

	// to create the filenames in sequence for save the process frames
	std::stringstream ss;
	ss << "D:/A7_measurement_14.4.20/img_process/process_2/" << i << ".png";

	// Load the frame to process
    //frame = imread(fn[iFrameCount].c_str(), IMREAD_COLOR);

	// Calibration images 14.04.2020_A7_Q5
    //frame = imread("D:/A7_measurement_14.4.20/cal/4672_5cm.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/cal/1035_12.5cm.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/cal/178_47cm.png", IMREAD_COLOR);
	
	// Process frames ZIM 212
	//frame = imread("D:/A7_measurement_14.4.20/img/4294.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/4296.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/4437.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/4457.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/4461.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/4485.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/4557.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/4600.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/4605.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/4638.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/5244.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/6384.png", IMREAD_COLOR);
	frame = imread("D:/A7_measurement_14.4.20/img/6472.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/6584.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/6813.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/7018.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/7888.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/8371.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/8916.png", IMREAD_COLOR);
	//frame = imread("D:/A7_measurement_14.4.20/img/8946.png", IMREAD_COLOR);
	
	printf("\nfile: %s", fn[iFrameCount].c_str());

    // If the frame is empty, break immediately
    if (frame.empty()) break;

	// Resize the frame
	//cv::resize(frame, resize_frame, Size(1280, 720));
	//cv::resize(frame, frame, Size(1280, 720));

	imshow("img", frame);

	//cvtColor(frame, HSV_Img, COLOR_BGR2HSV);

	//imshow("img_HSV1", HSV_Img);

	// Convert the GT_D2L in mm
	GT_D2L = 33.71 * 10;
	
	// Convert the D2L in to pixels
	pix_lane = GT_D2L / PIXEL_DIST;
	
	// GT point on Lnae (x,y)
	GT_point_lane_x = START_X + pix_lane;
	GT_point_lane_y = START_Y;

	// create the points (x1, y1, x2, Y2) fro ROI
	ROI_x1 = GT_point_lane_x - 20;
	ROI_y1 = GT_point_lane_y - 20;
	ROI_x2 = 40;
	ROI_y2 = 40;
	
	printf("\nGT_D2L: %d mm, Pix_ROI: %d", GT_D2L, pix_lane);
	
	printf("\nROI: X1= % d, Y1= % d, x2= % d,Y2= % d", ROI_x1, ROI_y1, ROI_x2, ROI_y2);

	// Here we define our region of interest
	//Rect const box(247, 50, 293, 350); // 47 cm > calibration // process_2

	Rect const box(ROI_x1, ROI_y1, ROI_x2, ROI_y2); // 47 cm > calibration // process_2

	Mat ROI = frame(box);

	// convert our fame to HSV Space
	cvtColor(ROI, HSV_Img, COLOR_BGR2HSV);

	imshow("img_HSV", HSV_Img);

	// white color thresholding
	Scalar whiteMinScalar = Scalar(100, 100, 50); // 10 0 90 // 100 100 60 //
	Scalar whiteMaxScalar = Scalar(255, 189, 255); // 50 189 255 // 250 189 255

	inRange(ROI, whiteMinScalar, whiteMaxScalar, LinesImg);
	
	imshow("InRange", LinesImg);

	// Edge detection using canny detector
	int minCannyThreshold = 190; // 190
	int maxCannyThreshold = 230; // 230
	Canny(LinesImg, LinesImg, minCannyThreshold, maxCannyThreshold, 3, true);

	// Morphological Operation
	Mat k = getStructuringElement(MORPH_RECT, Size(5, 5)); //MATLAB :k=Ones(9) // process_1 

	morphologyEx(LinesImg, LinesImg, MORPH_CLOSE, k);
	
	// now applying hough transform TO dETECT Lines in our image
	vector<Vec4i> lines;

	//HoughLinesP(LinesImg, lines, 1, CV_PI / 180, 150, 0, 50); // process_1
	HoughLinesP(LinesImg, lines, 1, CV_PI / 180, 5, 0, 10); // process_2
	
	/*
	double rho = 1;
	double theta = CV_PI / 180;
	int threshold = 150;     // 43 // 100 // 180
	double minLinLength = 0; // 0
	double maxLineGap = 50;  // 200
	
	//HoughLinesP(LinesImg, lines, rho, theta, threshold, minLinLength, maxLineGap);
	*/
	
	// Draw our lines
	//printf("lines: %d\n\n", lines.size());

	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];  // we have 4 elements p1=x1,y1  p2= x2,y2
		Scalar greenColor = Scalar(0, 255, 0);  // B=0 G=250 R=30
		line(ROI, Point(l[0], l[1]), Point(l[2], l[3]), greenColor, 1.5, 4);
	}

	//imshow("ROI_Lines", ROI);

	Canny(frame, LinesImg, 250, 255, 3, true);
		
	imshow("canny 2", LinesImg);
	
	// process D2L
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

		//if (iLanePixelCount > iLanePixelThreshold) {
		if (pixel >= iLaneColorUpperThreshold) {
			// Find the D2L
			//i -= iLanePixelThreshold;

			Point2f a(START_X, START_Y);
			Point2f b(i, END_Y);
			
			int result = cv::norm(cv::Mat(b), cv::Mat(a));

			//float distance = result * 1.15 * 0.1; // 0.26 // 0.41 // Procoess_1

			float distance = result * PIXEL_DIST * 0.1; // 0.26 // 0.41 // Procoess_2

			printf("\nx:%d, y:%d , val= %u \n", i, END_Y, pixel);
			printf("We got the lane, found D2L= %f cm\n", distance);

			// Display the Frames
			line(frame, Point(START_X, START_Y), Point(i, END_Y), Scalar(255, 255, 0), 1, 8);
			//line(edges, Point(START_X, END_Y), Point(i, END_Y), Scalar(255, 255, 255), 1, 8);

			// put the txt Frame number on Resize frame for information
			//putText(frame, format("Frame: %s", fn[iFrameCount].c_str()), Point(0, 700), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

			// put the text Frame number on real frame for information
			putText(frame, format("Frame: %s", fn[iFrameCount].c_str()), Point(0, 450), FONT_HERSHEY_PLAIN, 0.75, Scalar(255, 255, 255));

			// put the text D2L on Resize frame for information
			//putText(frame, format(" D2L: %f cm", distance), Point(50, 670), FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255));

			// put the text D2L on Real frame for information
			putText(frame, format(" D2L: %f cm", distance), Point(0, 400), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));

			// add the Adrive logo
			adrive_logo = imread("ADrive_Logo1.png", IMREAD_COLOR);
			//adrive_logo = imread("HS_Kempten_Logo.jpg", IMREAD_COLOR);

			// Put the LOGO on resize frame
			cv::resize(adrive_logo, new_logo, Size(135, 50));
			//new_logo.copyTo(frame(cv::Rect(1120, 650, new_logo.cols, new_logo.rows)));

			// Put the LOGO on real frame
			new_logo.copyTo(frame(cv::Rect(500, 420, new_logo.cols, new_logo.rows)));
			
			// Write the process METADATA in .csv file
			//fprintf(file, "%s, %f\n", fn[iFrameCount].c_str(), distance);

			// save process_frame as image
			//imwrite(ss.str().c_str(), frame);
			//imwrite("process_6472.png", frame);

			// Write to video file
			//wrOutVideo.write(frame);
			
			// Display the original frame or process frame
			imshow("Process_frame_D2L", frame);

			break;
		}
	}
		
	fclose(file);
	
	if (waitKey(0) == 27)
	{
		cout << "esc";
		break;
	}

	iFrameCount ++;
	i++;
  } // loop

  // Closes all the frames
  destroyAllWindows();

  return 0;
}
