// open_image.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

int open_uyvy(char *fileName, int fW, int fH)
{
    if (fileName == NULL)
        return -1;

    // std::string image_path = samples::findFile("./data/sample.jpg");
    std::string image_path = samples::findFile(fileName);
    std::cout << "Open image: !!\n";

    FILE *f = fopen(fileName, "rb");
    
    if (f == NULL) {
        std::cout << fileName << ": could not be opened";
        return -1;
    }

    //int fW = 1920;
    //int fH = 1080;
    int fC = 2;
    int fS = fW * fH * fC;
    char *databuffer = new char[fS];
    //VideoCapture cap(image_path);
    //if (!cap.isOpened()) // check if we succeeded
    //    return -1;

    int ret_val = fread(databuffer, 1, fS, f);

    // Mat objet with raw uyvy data
    cv::Mat mSrc(Size(fW, fH),CV_8UC2, databuffer);
     
    //cv::Mat OpencvImage(Size(fW, fH), CV_8UC3, databuffer, Mat::AUTO_STEP);

    // Mat output object buffer creation
    cv::Mat OpencvImage(Size(fW, fH), CV_8UC3);

    // COLOR_YUV2BGR_UYVY
    // Mat output data conversion
    //cvtColor(mSrc, OpencvImage, COLOR_YUV2BGR_YUYV, 3);
    cvtColor(mSrc, OpencvImage, COLOR_YUV2BGR_UYVY, 3);

    imshow ("Sample", OpencvImage);
    int k = waitKey(0); // Wait for a keystroke in the window

    //Mat edges;
    //namedWindow("edges", 1);
    //for (;;)
    //{
    //    Mat frame;
    //    cap >> frame; // get a new frame from camera
    //    cv::cvtColor(frame, edges, COLOR_BGR2GRAY);
    //    GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
    //    Canny(edges, edges, 0, 30, 3);
    //    imshow("edges", edges);
    //    if (waitKey(30) >= 0)
    //        break;
    //}
    // the camera will be deinitialized automatically in VideoCapture destructor
    // Mat img = imread(image_path, IMREAD_COLOR);
    // if(img.empty())
    //{
    //    std::cout << "Could not read the image: " << image_path << std::endl;
    //    return 1;
    //}
    // imshow("Display window", img);
    // int k = waitKey(0); // Wait for a keystroke in the window
    // if(k == 's')
    //{
    //    imwrite("starry_night.png", img);
    //}
    return 0;
}