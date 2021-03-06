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

struct parameters
{
	//ROI Parameters
    int height;
	int width;
	int x_coordinate;
	int y_coordinate;
	
    //output as a flag
	bool output1 = true;
	bool output2 = false;

	//which color to detect
	int color;
};

void get_input_parameters(parameters *Ptr, cv::Mat img)
{
	parameters P;
	cout << "Enter the value of top left x coordinate:";
	cin >> Ptr->x_coordinate;
	cout << "Enter the value of top left y coordinate:";
	cin >> Ptr->y_coordinate;
	cout << "Enter the value of rectangle width:";
	cin >> Ptr->width;
	cout << "Enter the value of rectangle height:";
	cin >> Ptr->height;
	Rect2i myROI(Ptr->x_coordinate, Ptr->y_coordinate, Ptr->width, Ptr->height);
	ROI = img(myROI);
}

void detect_light(parameters *ptr, cv::Mat ROI)
{
	parameters P;
	cv::Mat image_hsv;
	cv::Mat lower_hue_range;
	cv::Mat upper_hue_range;
	cv::Mat hue_image;
	cv::medianBlur(ROI, ROI, 3);
	cv::cvtColor(ROI, image_hsv, COLOR_BGR2HSV);

	cout << "Press 1 to detect red light" << endl << endl;
	cout << "Press 2 to detect Green light" << endl << endl;
	cin >> ptr->color;

	if (ptr->color == 1)
	{
		cv::inRange(image_hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_hue_range);
		cv::inRange(image_hsv, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_hue_range);
	}

	if (ptr->color == 2)
	{

		cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(86, 255, 255), lower_hue_range);
		cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(70, 255, 255), upper_hue_range);
    }

	if (ptr->color >= 3)
	{
		printf("Invalid color selection\n");
		// std::cout << std::boolalpha << P.output2 << '\n'; // For getting output as "false" 
		std::cout << std::noboolalpha << P.output2 << '\n';
		return void();
		
	}
    
	if (ptr->color <= 0)
	{
		printf("Invalid color selection\n");
		std::cout << std::noboolalpha << P.output2 << '\n';
		return void();
	}

	cv::addWeighted(lower_hue_range, 1.0, upper_hue_range, 1.0, 0.0, hue_image);
	cv::GaussianBlur(hue_image, hue_image, cv::Size(9, 9), 2, 2);
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(hue_image, circles, HOUGH_GRADIENT, 1, hue_image.rows / 8, 100, 27, 0, 0);
	if (circles.size() == 0)
	{
		std::cout << std::noboolalpha << P.output2 << '\n';
		std::exit(-1);
	}
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);
		std::cout << std::noboolalpha << P.output1 << '\n';
		cv::circle(ROI, center, radius, cv::Scalar(139, 0, 0), 3);

		cv::namedWindow("Detected input image", cv::WINDOW_AUTOSIZE);
		cv::imshow("Detected input image", ROI);
		cv::namedWindow("Detected hsv image", cv::WINDOW_AUTOSIZE);
		cv::imshow("Detected hsv image", image_hsv);

	}

	return void();
}

int main(int argc, char** argv)
{
	parameters P;
	char name[50];
	while (1)
	{
		for (int i = 0; i <= 2; i++)
		{
			sprintf_s(name, "C:/images/image%d.png", i);
			Mat img = imread(name, 1);
			if (img.empty())
			{
				printf("image not loaded");
				break;
			}
			get_input_parameters(&P, img);
			detect_light(&P, ROI);
			ofstream myfile;
			myfile.open("output.txt");
			myfile << "Red or Green light is detected\n";
			myfile.close();
			waitKey(1);
		}
	}
	
	return 0;
}

