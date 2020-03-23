#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <string>
#include "../../../library/include/opencv2/highgui/highgui_c.h"
using namespace cv;
using namespace std;
Mat src, img, ROI, image, image_hsv;
cv::Mat lower_red_hue_range;
cv::Mat upper_red_hue_range;
cv::Mat red_hue_image;
Rect cropRect(0, 0, 0, 0);
Point P1(0, 0);
Point P2(0, 0);

const char* winName = "Crop Image";
bool clicked = false;
int i = 0;
char imgName[15];


void checkBoundary() {
	//check croping rectangle exceed image boundary
	if (cropRect.width > img.cols - cropRect.x)
		cropRect.width = img.cols - cropRect.x;

	if (cropRect.height > img.rows - cropRect.y)
		cropRect.height = img.rows - cropRect.y;

	if (cropRect.x < 0)
		cropRect.x = 0;

	if (cropRect.y < 0)
		cropRect.height = 0;
}

void showImage() {
	img = src.clone();
	checkBoundary();
	if (cropRect.width > 0 && cropRect.height > 0) {
		ROI = src(cropRect);
		imshow("cropped", ROI);
	}

	rectangle(img, cropRect, Scalar(0, 255, 0), 1, 8, 0);
	imshow(winName, img);
}


void onMouse(int event, int x, int y, int f, void*) {


	switch (event) {

	case  CV_EVENT_LBUTTONDOWN:
		clicked = true;

		P1.x = x;
		P1.y = y;
		P2.x = x;
		P2.y = y;
		break;

	case  CV_EVENT_LBUTTONUP:
		P2.x = x;
		P2.y = y;
		clicked = false;
		break;

	case  CV_EVENT_MOUSEMOVE:
		if (clicked) {
			P2.x = x;
			P2.y = y;
		}
		break;

	default:   break;


	}


	if (clicked) {
		if (P1.x > P2.x) {
			cropRect.x = P2.x;
			cropRect.width = P1.x - P2.x;
		}
		else {
			cropRect.x = P1.x;
			cropRect.width = P2.x - P1.x;
		}

		if (P1.y > P2.y) {
			cropRect.y = P2.y;
			cropRect.height = P1.y - P2.y;
		}
		else {
			cropRect.y = P1.y;
			cropRect.height = P2.y - P1.y;
		}

	}


	showImage();


}
int main()
{
	
	cout << "Click and drag for Selection" << endl << endl;
	cout << "------> Press 's' to save" << endl << endl;

	cout << "------> Press '8' to move up" << endl;
	cout << "------> Press '2' to move down" << endl;
	cout << "------> Press '6' to move right" << endl;
	cout << "------> Press '4' to move left" << endl << endl;

	cout << "------> Press 'w' increas top" << endl;
	cout << "------> Press 'x' increas bottom" << endl;
	cout << "------> Press 'd' increas right" << endl;
	cout << "------> Press 'a' increas left" << endl << endl;

	cout << "------> Press 't' decrease top" << endl;
	cout << "------> Press 'b' decrease bottom" << endl;
	cout << "------> Press 'h' decrease right" << endl;
	cout << "------> Press 'f' decrease left" << endl << endl;

	cout << "------> Press 'r' to reset" << endl;
	cout << "------> Press 'Esc' to quit" << endl << endl;


	src = imread("C:/output/dsBuffer.png", 1);

	namedWindow(winName, WINDOW_NORMAL);
	setMouseCallback(winName, onMouse, NULL);
	imshow(winName, src);

	while (1)
	{
		char c = waitKey();
		printf("Check\n");
		if ((c >= 'a') && (c >= 'z'))
		{
			printf("%c", c);
		}
		
		if (c == 's' && ROI.data) {
			sprintf_s(imgName, "%d.jpg", i++);
			imwrite(imgName, ROI);
			cout << "  Saved " << imgName << endl;
		}
		if (c == '6') cropRect.x++;
		if (c == '4') cropRect.x--;
		if (c == '8') cropRect.y--;
		if (c == '2') cropRect.y++;

		if (c == 'w') { cropRect.y--; cropRect.height++; }
		if (c == 'd') cropRect.width++;
		if (c == 'x') cropRect.height++;
		if (c == 'a') { cropRect.x--; cropRect.width++; }

		if (c == 't') { cropRect.y++; cropRect.height--; }
		if (c == 'h') cropRect.width--;
		if (c == 'b') cropRect.height--;
		if (c == 'f') { cropRect.x++; cropRect.width--; }

		if (c == 27) break;
		if (c == 'r') { cropRect.x = 0;cropRect.y = 0;cropRect.width = 0;cropRect.height = 0; }
		showImage();
		int num;
		cout << "Press 1 to detect red light" << endl << endl;
		cout << "Press 2 to detect Green light" << endl << endl;
		cin >> num;
		if (num ==1) {

			cv::medianBlur(ROI, image, 3);

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
		}
		
        if (num == 2) {

			cv::medianBlur(ROI, image, 3);

			cv::cvtColor(image, image_hsv, COLOR_BGR2HSV);
			printf("Check_HSV\n");

			cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(86, 255, 255), lower_red_hue_range);
			cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(70, 255, 255), upper_red_hue_range);
			cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);
			cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);
			std::vector<cv::Vec3f> circles;
			cv::HoughCircles(red_hue_image, circles, HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 27, 0, 0);
			if (circles.size() == 0) std::exit(-1);
			for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
				cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
				int radius = std::round(circles[current_circle][2]);

				cv::circle(image, center, radius, cv::Scalar(139, 0, 0), 3);
				cv::imshow("Detected red circles on the input image", image);

			}
		}

		if (num >= 3) {

			printf("Invalid color selection\n");

		}

		if (num <= 0) {

			printf("Invalid color selection\n");

		}
		
	}

	return 0;
}

/*int main(int argc, char** argv)
{
	Mat image;
	image = imread("C:/output/image-100.png", IMREAD_COLOR); // Read the file
	int x, y, w, h;
	cout << "Enter the value of top left x coordinate:";
	cin >> x;
	cout << "Enter the value of top left y coordinate:";
	cin >> y;
	cout << "Enter the value of rectangle width:";
	cin >> w;
	cout << "Enter the value of rectangle height:";
	cin >> h;
	Rect myROI(x, y, w, h);
	rectangle(image, myROI, cv::Scalar(255, 0, 0), 2, 8, 0);
    namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", image);  // Show our image inside it.
	waitKey(0);
	return 0;
}*/
/*#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../../../library/include/opencv2/highgui/highgui_c.h"
#include <vector>
#include <iostream>

using cv::WINDOW_NORMAL;
using cv::imread;
using cv::waitKey;
using cv::setMouseCallback;
using cv::imshow;
using cv::Rect;
using cv::Point;
using cv::Scalar;
using cv::rectangle;
using cv::Mat;

Mat source_image;
Mat img;

// Current rectangle and start&end points
Rect rect(0, 0, 0, 0);
Point P1(0, 0);
Point P2(0, 0);

// Rectangles to be outputted
std::vector<Rect> rects;

static const char* WINDOW_NAME = "Tracking Info Generator";
static bool clicked = false;


void fixBoundaries() {
	if (rect.width > img.cols - rect.x)
		rect.width = img.cols - rect.x;

	if (rect.height > img.rows - rect.y)
		rect.height = img.rows - rect.y;

	if (rect.x < 0)
		rect.x = 0;

	if (rect.y < 0)
		rect.height = 0;
}

void draw() {
	img = source_image.clone();
	fixBoundaries();

	for (const auto& r : rects)
		rectangle(img, r, Scalar(0, 255, 0), 1, 8, 0);
	rectangle(img, rect, Scalar(0, 255, 0), 1, 8, 0);

	imshow(WINDOW_NAME, img);
}


void onMouse(int event, int x, int y, int f, void*) {
	switch (event) {
	case CV_EVENT_LBUTTONDOWN:
		clicked = true;
		P1.x = x;
		P1.y = y;
		P2.x = x;
		P2.y = y;
		break;

	case  CV_EVENT_LBUTTONUP:
		clicked = false;
		P2.x = x;
		P2.y = y;
		break;

	case CV_EVENT_MOUSEMOVE:
		if (clicked) {
			P2.x = x;
			P2.y = y;
		}
		break;

	default:
		break;
	}

	if (P1.x > P2.x) {
		rect.x = P2.x;
		rect.width = P1.x - P2.x;
	}
	else {
		rect.x = P1.x;
		rect.width = P2.x - P1.x;
	}

	if (P1.y > P2.y) {
		rect.y = P2.y;
		rect.height = P1.y - P2.y;
	}
	else {
		rect.y = P1.y;
		rect.height = P2.y - P1.y;
	}

	draw();
}

int main()
{
	source_image = imread("C:/output/lll.png", 1);

	namedWindow(WINDOW_NAME, WINDOW_NORMAL);
	setMouseCallback(WINDOW_NAME, onMouse, NULL);
	imshow(WINDOW_NAME, source_image);

	while (1) {
		char c = waitKey();

		switch (c) {
		case 's':
			if (rects.empty()) {
				std::cerr << "No rect added." << std::endl
					<< "Select an area and press 'a' to add!" << std::endl;
				continue;
			}

			for (const auto& r : rects) {
				std::cout << r.x + r.width / 2 << " "
					<< r.y + r.height / 2 << std::endl;
			}
			break;
		case 'a':
			rects.push_back(rect);
			rect = Rect(0, 0, 0, 0);
			break;
		}
	}

	return 0;
}*/