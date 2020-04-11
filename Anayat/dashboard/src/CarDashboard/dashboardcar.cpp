#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
using namespace cv;
using namespace std;
Mat ROI;
Mat img;
Mat image_hsv;
int detect_light(Mat ROI);

struct parameters
{
	int x, y, w, h;
};

void getinputparameters(cv::Mat img)
{
	parameters P;
	cout << "Enter the value of top left x coordinate:";
	cin >> P.x;
	cout << "Enter the value of top left y coordinate:";
	cin >> P.y;
	cout << "Enter the value of rectangle width:";
	cin >> P.w;
	cout << "Enter the value of rectangle height:";
	cin >> P.h;
	Rect2i myROI(P.x, P.y, P.w, P.h);
	ROI = img(myROI);
}

int main(int argc, char** argv)
{
	parameters P;
	char name[50];
	int i = 0;
	while (1)
	{
	    sprintf_s(name, "C:/images/image%d.png", i);
		Mat img = imread(name, 1);
		if (img.empty())
		{
			printf("image not loaded");
			break;
		}
		//img = imread("C:/output/aaa.png", IMREAD_COLOR); // Read the file
		getinputparameters(img);
		detect_light(ROI);
		ofstream myfile;
		myfile.open("output.txt");
		myfile << "Red or Green light is detected\n";
		myfile.close();
		i++;
		waitKey(0);
	}
	
	return 0;
}

int detect_light(Mat ROI)
{
	parameters P;
	cv::Mat image_hsv;
	cv::Mat lower_hue_range;
	cv::Mat upper_hue_range;
	cv::Mat hue_image;
	int num;
	cv::medianBlur(ROI, ROI, 3);
    cv::cvtColor(ROI, image_hsv, COLOR_BGR2HSV);

	cout << "Press 1 to detect red light" << endl << endl;
	cout << "Press 2 to detect Green light" << endl << endl;
	cin >> num;
	
	if (num == 1) 
	{
		cv::inRange(image_hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_hue_range);
        cv::inRange(image_hsv, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_hue_range);
    }

	if (num == 2)
	{

		cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(86, 255, 255), lower_hue_range);
		cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(70, 255, 255), upper_hue_range);
		
    }

	if (num >= 3) 
	{
       printf("Invalid color selection\n");
	   return 0;
    }
	
	if (num <= 0)
	{
       printf("Invalid color selection\n");
	   return 0;
	}
	
    cv::addWeighted(lower_hue_range, 1.0, upper_hue_range, 1.0, 0.0, hue_image);
	cv::GaussianBlur(hue_image, hue_image, cv::Size(9, 9), 2, 2);
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(hue_image, circles, HOUGH_GRADIENT, 1, hue_image.rows / 8, 100, 27, 0, 0);
	if (circles.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);

		cv::circle(ROI, center, radius, cv::Scalar(139, 0, 0), 3);
		cv::namedWindow("Detected input image", cv::WINDOW_AUTOSIZE);
		cv::imshow("Detected input image", ROI);
		cv::namedWindow("Detected hsv image", cv::WINDOW_AUTOSIZE);
		cv::imshow("Detected hsv image", image_hsv);

	}

	return 0;
}