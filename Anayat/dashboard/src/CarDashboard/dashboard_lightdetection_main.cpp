#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
using namespace cv;
using namespace std;
int detect_redlight(struct Redlight R);
int detect_greenlight(struct Redlight R);
struct Redlight
{
	int x, y, w, h;
	


};
int main(int argc, char** argv)
{
	Redlight R;
	
	cout << "Enter the value of top left x coordinate:";
	cin >> R.x;
	cout << "Enter the value of top left y coordinate:";
	cin >> R.y;
	cout << "Enter the value of rectangle width:";
	cin >> R.w;
	cout << "Enter the value of rectangle height:";
	cin >> R.h;
	int num;
	cout << "Press 1 to detect red light" << endl << endl;
	cout << "Press 2 to detect Green light" << endl << endl;
	cin >> num;
	if(num == 1){

		detect_redlight(R);

	}
	if(num == 2){ 
       
		detect_greenlight(R);

	}
	if (num >= 3) {

		printf("Invalid color selection\n");

	}
	if (num <= 0) {

		printf("Invalid color selection\n");

	}
	
	
	
	waitKey(0);
	return 0;
	

}

int detect_redlight(struct Redlight R)
{
	Mat img;
	Mat image;
	Mat image_hsv;
	cv::Mat lower_red_hue_range;
	cv::Mat upper_red_hue_range;
	cv::Mat red_hue_image;

	img = imread("C:/output/aaa.png", IMREAD_COLOR); // Read the file
	

	Rect myROI(R.x, R.y, R.w, R.h);
	rectangle(img, myROI, cv::Scalar(255, 0, 0), 2, 8, 0);
	cv::Mat ROI_crop = img(myROI);
	
	cv::medianBlur(ROI_crop, image, 3);

	cv::cvtColor(image, image_hsv, COLOR_BGR2HSV);

	cv::inRange(image_hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);

	cv::inRange(image_hsv, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);

	cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
	cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(red_hue_image, circles, HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 20, 0, 0);
	if (circles.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);

		cv::circle(image, center, radius, cv::Scalar(0, 255, 0), 3);
		cv::imshow("Detected red circles on the input image", image);

	}
	
	return 0;
}

int detect_greenlight(struct Redlight R)
{
	Mat img;
	Mat image;
	Mat image_hsv;
	cv::Mat lower_green_hue_range;
	cv::Mat upper_green_hue_range;
	cv::Mat green_hue_image;
	img = imread("C:/output/aaa.png", IMREAD_COLOR); // Read the file


	Rect myROI(R.x, R.y, R.w, R.h);
	rectangle(img, myROI, cv::Scalar(255, 0, 0), 2, 8, 0);
	cv::Mat ROI_crop = img(myROI);

	cv::medianBlur(ROI_crop, image, 3);

	cv::cvtColor(image, image_hsv, COLOR_BGR2HSV);

	cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(86, 255, 255), lower_green_hue_range);
	cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(70, 255, 255), upper_green_hue_range);
	cv::addWeighted(lower_green_hue_range, 1.0, upper_green_hue_range, 1.0, 0.0, green_hue_image);
	cv::GaussianBlur(green_hue_image, green_hue_image, cv::Size(9, 9), 2, 2);
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(green_hue_image, circles, HOUGH_GRADIENT, 1, green_hue_image.rows / 8, 100, 27, 0, 0);
	if (circles.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);

		cv::circle(image, center, radius, cv::Scalar(139, 0, 0), 3);
		cv::imshow("Detected green circles on the input image", image);

	}

	return 0;




}
