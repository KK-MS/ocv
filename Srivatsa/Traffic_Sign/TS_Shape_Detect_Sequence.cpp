#include <iostream>
#include <stdio.h>

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

RNG rng(12345);
Mat src, img,ROI, gray, canny, HSV, Threshold;

//STructure to obtain parameters of the sign//

struct obj_parameter
{
	//Input Parameters from the Ground Truth//
	int gt_height;
	int gt_width;
	int gt_radius;
	int gt_diameter;
	int num;//Based on Shape
	int color; // Based on Color
	//Optional:- Localisation Parameters//
	double gt_lat;
	double gt_long;
	double gt_bearing;
	// From where the image was captured
	double img_lat;
	double img_long;
	double img_bearing;
	//ROI Parameters//
	int a;
	int b;
	int c;
	int d;
	//Output Parameters after detection (circle or any shape)//
	int height;
	int width;
	int radius; // only for circle
	int diameter; // only for circle
	float distance;
	//Color Parameters
	int LH;
	int HH;
};

//Function to set labels in images//

void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 2;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), 1);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(1, 1, 1), thickness, 8);
}
/*
Function to create ROI TODO
void create_ROI(cv::Mat frame)
{
	obj_parameter p;
	cout << "Enter the value of top left x coordinate:";
	cin >> p.a;
	cout << "Enter the value of top left y coordinate:";
	cin >> p.b;
	cout << "Enter the value of rectangle width:";
	cin >> p.c;
	cout << "Enter the value of rectangle height:";
	cin >> p.d;
	Rect2i box(p.a, p.b, p.c, p.d);
	ROI = src(box);
}
*/
//Function to detect circle//
obj_parameter detect_circle(Mat ROI, Mat gray)
{
	cvtColor(ROI, gray, COLOR_BGR2GRAY);

	// Hough circles
	GaussianBlur(gray, gray, Size(9, 9), 3, 3);
	vector<Vec3f> circles;
	HoughCircles(gray, circles, HOUGH_GRADIENT, 1, gray.rows, 250, 80, 0);

	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int rad_2 = cvRound(circles[i][2]);

		// draw the circle center
		cv::circle(ROI, center, 2, Scalar(0, 0, 0), -3, 8, 0);

		// draw the circle outline				
		cv::circle(ROI, center, rad_2, Scalar(0, 0, 0), 1, 8, 0);

		// Draw Rectangle

		int cx, cy, cr, bound = 10;
		cx = cvRound(circles[i][0]);
		cy = cvRound(circles[i][1]);
		cr = cvRound(circles[i][2]);
		int x = cx - cr - bound;
		int y = cy - cr - bound;
		int h = (cr + bound) * 2;
		int w = (cr + bound) * 2;

		Rect rec_save = cv::Rect2i(x, y, h, w);

		cv::rectangle(ROI, rec_save, Scalar(0, 0, 0), 1, 8, 0);
		ROI = ROI(rec_save);
		//Calculate Diameter//
		int d = (2 * cr);
		obj_parameter p;
		p.height = h;
		p.width = w;
		p.radius = cr;
		p.diameter = d;
		p.distance = (0.58 * 1512) / p.width;
		printf("\nHeight of sign: %d\n", p.height);
		printf("Width of sign: %d\n", p.width);
		printf("Radius of sign: %d\n", p.radius);
		printf("Diameter of sign: %d\n", p.diameter);
		printf("Distance to sign: %f\n", p.distance);
		return p;
	}
}

// Function to detect Rectangle//

void detect_Rectangle(Mat ROI, Mat gray, Mat canny)
{
	obj_parameter p;
	cvtColor(ROI, gray, COLOR_BGR2GRAY);
	cv::Canny(gray, canny, 0, 50, 5);

	/////Find Counters//////
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(canny, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;
	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
			continue;

		else if (approx.size() >= 4 && approx.size() <= 6)
		{
			// Number of vertices of polygonal curve
			approx.size() >= 4 && approx.size() <= 6;
			int vtc = approx.size();
			if (vtc == 4)
			{
				setLabel(ROI, "RECTANGLE", contours[i]);
				printf("contours_size = %f", contours[i]);
				printf("Rectangle Detected\n");
			}
		}
	}
}


int main(int argc, char* argv[])
{
	obj_parameter p;
	if (argc > 4)
	{
		printf("Too many arguments\n");
		return -1;
	}
	p.num = atoi(argv[1]); //Based on Shape
	p.color = atoi(argv[2]); // Based on Color
	int iFrameCount = 0;
	int i = 0;
	//float distance;
	// Load the image files from folder in sequence
	vector<cv::String> fn;
	glob("C:/Users/sriva/source/repos/Traffic_Sign_Detection/img/*.png", fn, false);	
	while (1) 
	{
		// Load the frame to process
		src = imread(fn[iFrameCount].c_str(), IMREAD_COLOR);
		printf("\nfile: %s", fn[iFrameCount].c_str());
		// If the frame is empty, break immediately
		if (src.empty())
		{
			printf("Frame not loaded. Breaking");
		}
		//create_ROI(src);
		//cv::resize(src, src, Size(640, 360));
		Rect const box(640, 0, 640, 360);
		ROI = src(box);
		if (p.num == 1)
		{
			detect_circle(ROI, gray);
			printf("Circle Detected_Function Called\n");
		}
		else if (p.num == 2)
		{
			detect_Rectangle(ROI, gray, canny);
			printf("Rectangle Detected_Function Called\n");
		}
		else
		{
			printf("Invalid Arguments\n");
		}
		if (p.color == 1)
		{
			cvtColor(ROI, HSV, COLOR_BGR2HSV);
			inRange(HSV, Scalar(150, 0, 0), Scalar(180, 255, 255), Threshold);
			printf("Color Red Detected\n");
		}
		else if (p.color == 2)
		{
			cvtColor(ROI, HSV, COLOR_BGR2HSV);
			inRange(HSV, Scalar(20, 0, 0), Scalar(30, 255, 255), Threshold);
		}
		else
		{
			printf("Invalid Color Arguments\n");
		}
		waitKey(100);


		// to create the filenames in sequence for save the process frames
		std::stringstream ss;
		ss << "C:/Users/sriva/source/repos/Traffic_Sign_Detection/img/frame_" << i << ".png";

		// Save the Process frame on device
		//imwrite(ss.str().c_str(), src);
		namedWindow("Input", WINDOW_AUTOSIZE);
		imshow("Input", src);

		char c = (char)waitKey(100);  // Press any KEY on Keyboard to continue
		if (c == 27) break;		    // Press ESC to exit

		iFrameCount++;
		i++;
	}
	

	// Closes all the frames
	destroyAllWindows();

	return 0;
}