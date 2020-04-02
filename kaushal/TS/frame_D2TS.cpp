#include <iostream>
#include <stdio.h>

#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

RNG rng(12345);

int main(int argc, char *argv[])
{
  int iFrameCount = 0;
  int i = 0;
  //float distance;
  
  Mat frame, resize_frame, edges, adrive_logo, new_logo , src_gray, canny_img, canny_ROI;
  Mat whiteLane, yellowLane, LinesImg, HSV_Img;

  // Load the image files from folder in sequence
  vector<cv::String> fn;
  //glob("D:/project/Kaushal/Basti_Visual_Studio_Projekt/TS_CM/cm_mockup_gt_odometry/img/145.png", fn, false);
  glob("D:/project/Kaushal/Basti_Visual_Studio_Projekt/TS_CM/cm_mockup_gt_odometry/img/*.png", fn, false);

  VideoWriter wrOutVideo;
  const String name = "./dist2TS.avi";
  wrOutVideo.open(name, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, Size(1280, 720), true);


  // Process the frames to find D2L and save the processed frames in device
  while(1) {

	float distance;

	// Load the frame to process
    frame = imread(fn[iFrameCount].c_str(), IMREAD_COLOR);
	
	printf("\nfile: %s", fn[iFrameCount].c_str());

    // If the frame is empty, break immediately
    if (frame.empty()) break;

	//cv::resize(frame, resize_frame, Size(1280, 720));
	cv::resize(frame, frame, Size(1280, 720));

	//cvtColor(frame, src_gray, COLOR_BGR2GRAY);

	//Canny(src_gray, canny_img, 50, 255, 3, true);
	//imshow("canny1", canny_img);

	//here we define our region of interest
	
	Rect const box(600, 150, 680, 400); //(x,y)=(100,295)
	                                    // and the second corner is (x + b, y+c )= (100 +400,295+185)

	//Rect const box(0, 0, 1280, 550);

	Mat ROI = frame(box);

	cvtColor(ROI, src_gray, COLOR_BGR2GRAY);

// Contours

#if 1

	//Canny(src_gray, canny_ROI, 200, 240, 3, true);
	Canny(src_gray, canny_ROI, 80, 240); // , 3, true);

	imshow("canny_ROI", canny_ROI);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	
	
	/// Find contours
	//findContours(canny_ROI, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//findContours(canny_ROI.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	findContours(canny_ROI.clone(), contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f>centers(contours.size());
	vector<float>radius(contours.size());
	cv::Rect bounding_rect;
#if 0
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], contours_poly[i], 3, true);
		//approxPolyDP(cv::Mat(contours[i]), contours_poly[i], cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);
		//boundRect[i] = boundingRect(contours_poly[i]);
		
		//minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
		
		// Skip small or non-convex objects
		//if (std::fabs(cv::contourArea(contours[i])) < 50 || !cv::isContourConvex(contours_poly[i]))
			//continue;
	}

	//Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	
	for (size_t i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		//drawContours(frame, contours_poly, (int)i, color);
		rectangle(frame, boundRect[i].tl(), boundRect[i].br(), color, 2);
		//circle(frame, centers[i], (int)radius[i], color, 2);
	}
#endif

#if 1
	for (size_t i = 0; i < contours.size(); i++)
	{
	
		//printf("\n [%d]", i);
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

		// Skip small or non-convex objects
		if (std::fabs(cv::contourArea(contours[i])) < 430 || !cv::isContourConvex(contours_poly[i]))
			continue;

		int largest_area = 0;
		int largest_contour_index = 0;
		
		double area = contourArea(contours[i], false);    // Find the area of contour
		if (area > largest_area) {
			largest_area = area;
			largest_contour_index = i;                 // Store the index of largest contour
			bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour

			distance = (0.58 * 1511.62) / bounding_rect.width;

			printf("\narge_area: %d", largest_area);
			printf("\nwidth: %d", bounding_rect.width);
			printf("\nD2TS: %f m\n", distance);

			putText(frame, format(" D2TS: %f m", distance), Point(50, 670), FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255));
			
			break;
		}
		
		// Detect and label circles
		//double area = cv::contourArea(contours[i]);
		//boundRect[i] = cv::boundingRect(contours[i]);
		//int radius = r.width / 2;

		//rectangle(frame, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 1, 8, 0);
		//rectangle(ROI, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);

		//if (largest_area > 0)
			//break;
	}

	rectangle(ROI, bounding_rect, Scalar(25, 205, 255), 1, 8, 0);

	//if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 && std::abs(1 - (area / (CV_PI * (radius * radius)))) <= 0.2)
		//setLabel(dst, "CIR", contours[i]);
#endif

	// put the txt on frame for information
	putText(frame, format("Frame: %s", fn[iFrameCount].c_str() ), Point(50, 50), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));

	// put the txt on frame for information
	//putText(frame, format(" D2TS: %f m", distance), Point(50, 670), FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255));

	// add the Adrive logo
	adrive_logo = imread("ADrive_Logo1.png", IMREAD_COLOR);
	//adrive_logo = imread("HS_Kempten_Logo.jpg", IMREAD_COLOR);

	cv::resize(adrive_logo, new_logo, Size(135, 50));

	//adrive_logo.copyTo(resize_frame(cv::Rect(100, 100, adrive_logo.cols, adrive_logo.rows)));
	new_logo.copyTo(frame(cv::Rect(1120, 650, new_logo.cols, new_logo.rows)));

	// Write to video file
	wrOutVideo.write(frame);

	imshow("Contours", frame);

	waitKey(100);

	/*
	int largest_area = 0;
	int largest_contour_index = 0;
	cv::Rect bounding_rect;

	for (int i = 0; i < contours.size(); i++)          // iterate through each contour.
	{
		double a = contourArea(contours[i], false);    // Find the area of contour
		if (a > largest_area) {
			largest_area = a;
			largest_contour_index = i;                 // Store the index of largest contour
			bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
			
		}

	} 
	
	drawContours(ROI, contours, largest_contour_index, Scalar(255, 0, 0), FILLED, 8, hierarchy); // Draw the largest contour using previously stored index.

	//rectangle(canny_ROI, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);
	rectangle(frame, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);

	*/

	
#endif


// Hough circles

#if 0    

	//medianBlur(src_gray, canny_ROI, 5);
	GaussianBlur(src_gray, canny_ROI, Size(3, 3), 0, 0);

	//Canny(src_gray, canny_ROI, 100, 80, 3, true);

	imshow("canny_ROI", canny_ROI);

	vector<Vec3f> circles;
	HoughCircles(canny_ROI, circles, HOUGH_GRADIENT, 1, canny_ROI.rows, 250, 80, 0);

	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int rad_2 = cvRound(circles[i][2]);

		// draw the circle center
		//cv::circle(ROI, center, 2, Scalar(0, 0, 0), -1, 8, 0);
		
		// draw the circle outline				
		//cv::circle(ROI, center, rad_2, Scalar(0, 0, 0), 2, 8, 0);

		// Draw Rectangle

		int cx, cy, cr, bound = 0;
		cx = cvRound(circles[i][0]); // TODO use latest 3.4.
		cy = cvRound(circles[i][1]); // TODO use latest 3.4.
		cr = cvRound(circles[i][2]); // TODO use latest 3.4.
		int x = cx - cr - bound;
		int y = cy - cr - bound;
		int h = (cr + bound) * 2;
		int w = (cr + bound) * 2;

		Rect rec_save = cv::Rect2i(x, y, h, w);

		//cv::rectangle(ROI, rec_save, Scalar(0, 0, 255), 4, 8, 0);
		cv::rectangle(frame, rec_save, Scalar(0, 0, 255), 1.5, 8, 0);
		//frame = ROI(rec_save);

		//Calculate Diameter//
		int d = (2 * cr);

		float distance = (0.58 * 1531) / d;   // Distance = ( width of object x Focallenght) / widtth of object in pixel
											  // FocalLength = (Distance x widtth of object in pixel) / width of object

		printf("\nFrame: %s\n", fn[iFrameCount].c_str());
		printf("\n cx: %d\n cy: %d\n cr: %d\n x: %d\n y: %d\n h: %d\n w: %d\n Dia: %d\n", cx, cy, cr, x, y, h, w, d);

		printf("\nD2TS: %f m\n", distance);
  }
#endif


#if 0
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
		//Scalar color(0, 255, 0);
		drawContours(canny_ROI, contours, i, color, 2, 8, hierarchy, 0, Point());
	}


	// Show in a window
	//namedWindow("Contours1", WINDOW_AUTOSIZE);

	//imshow("ROI", canny_ROI);

	//imshow("Contours1", frame);

		
	if (waitKey(0) == 27)
	{
		cout << "esc";
		break;
	}
#endif	
	
	// to create the filenames in sequence for save the process frames
	std::stringstream ss;
	ss << "D:/project/Kaushal/Basti_Visual_Studio_Projekt/TS_CM/cm_mockup_gt_odometry/img/process/frame_" << i << ".png";
	
	// Save the Process frame on device
	//imwrite("C:/Users/ll_stkapate/Desktop/Network/Network/SL_30s_Process/frame_599.jpg", frame);
	imwrite(ss.str().c_str(), frame);
	
     char c = (char)waitKey(100);  // Press any KEY on Keyboard to continue
     if(c == 27) break;		    // Press ESC to exit

    iFrameCount ++;
	i++;
  }

  // Closes all the frames
  destroyAllWindows();

  return 0;
}
