#include <iostream>.
#include "opencv2/opencv.hpp"
#include <stdio.h>

using namespace std;
using namespace cv;
char imgName[50];

Mat src, img, ROI, gray, canny, HSV;
Mat Threshold;
int frame_count = 0;

//Function to create ROI TODO
void create_ROI(cv::Mat src)
{
	int a, b, c, d;
	cout << "Enter the value of top left x coordinate:";
	cin >> a;
	cout << "Enter the value of top left y coordinate:";
	cin >> b;
	cout << "Enter the value of rectangle width:";
	cin >> c;
	cout << "Enter the value of rectangle height:";
	cin >> d;
	img = src.clone();
	Rect2i box(a,b,c,d);
	ROI = src(box);
}
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

//Function to detect circle//

int detect_circle(Mat ROI)
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
		cv::circle(ROI, center, 2, Scalar(255, 0, 0), -1, 8, 0);

		// draw the circle outline				
		cv::circle(ROI, center, rad_2, Scalar(0, 255, 0), 2, 8, 0);

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

		cv::rectangle(ROI, rec_save, Scalar(0, 0, 255), 4, 8, 0);
		ROI = ROI(rec_save);

		//Calculate Diameter//
		int d = (2 * cr);
		//printf("cx, cy, cr, x, y, h, w, %d, %d, %d, %d, %d, %d,  %d,\n", cx, cy, cr, x, y, h, w);
		printf("cx = %d\n", cx); //Center Point X of circle
		printf("cy = %d\n", cy); // Center point Y of circle // Point = (X,Y)
		printf("cr = %d\n", cr); // Radius of the circle

		//DImensions of Rectangle. x and y are the starting point on the top left of the rectangle
		//h and w are the length and width of the rectangle
		printf("x = %d\n", x);
		printf("y = %d\n", y);
		printf("h = %d\n", h);
		printf("w = %d\n", w);
		return d;
	}
}

//TODO Function to detect Rectangle//

void detect_Rectangle(Mat ROI)
{
	cv::Mat canny;
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

		/*if (approx.size() == 3)
		{
			approx.size() == 3;
			setLabel(ROI, "TRIANGLE", contours[i]);    // Triangles
			printf("Triangle detected\n");
		}*/
		
		else if (approx.size() >= 4 && approx.size() <= 6)
		{
			// Number of vertices of polygonal curve
			approx.size() >= 4 && approx.size() <= 6;
			int vtc = approx.size();
			if (vtc == 4)
			{
				setLabel(ROI, "RECTANGLE", contours[i]);
				printf("contours_size = %d", contours[i]);
				printf("Rectangle Detected\n");
			}
		}
	}
}

//Function to detect color//

void color_detect(Mat ROI, Mat HSV, Mat Threshold)
{
	int LH, LS, LV, HH, HS, HV;
	cout << "Enter the value of H Parameter, Low Hue:";
	cin >> LH;
	cout << "Enter the value of S Parameter, Low Saturation:";
	cin >> LS;
	cout << "Enter the value of V Parameter, Low Value:";
	cin >> LV;
	cout << "Enter the value of H Parameter, High Hue:";
	cin >> HH;
	cout << "Enter the value of S Parameter, High Saturation:";
	cin >> HS;
	cout << "Enter the value of V Parameter, High Value:";
	cin >> HV;
	cvtColor(ROI, HSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	inRange(HSV, Scalar(LH,LS, LV), Scalar(HH, HS, HV),	Threshold); //Threshold the image

}

int main(int argc, char* argv[])
{
	if (argc > 3)
	{
		printf("Too many arguments\n");
		return 1;
	}

	int num = atoi(argv[1]);
	int color = atoi(argv[2]);

	src = imread("C:/Users/sriva/source/repos/face_detection/face_detection/img/img/147.png");
	if (src.empty())
	{
		printf("Image not loaded\n");
		return -1;
	}

	while (1) 
	{
		create_ROI(src);
		if (num == 1)
		{
			cout << detect_circle(ROI);
			if (color == 1)
			{
				color_detect(ROI, HSV, Threshold);
				printf("Color_thresholded. Image window opened\n");
			}
			else
			{
				printf("Wrong parameter. Please Check Again.");
			}
			printf("Circle Detected_Function Called\n");
		}
		else if (num == 2);
		{
			detect_Rectangle(ROI);
			if (color == 2)
			{
				color_detect(ROI, HSV, Threshold);
				printf("Color_thresholded. Image window opened\n");
			}
			else
			{
				printf("Wrong parameter. Please Check Again.");
			}
		}		
		// Show in a window
		imshow("Input", src);
		imshow("cropped", ROI);
		imshow("Threshold", Threshold);
		cv::waitKey(0);
		return 0;
	}
}

