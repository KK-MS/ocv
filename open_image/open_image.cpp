// open_image.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

int open_image(char *fileName)
{
    if (fileName == NULL ) return -1;

    //std::string image_path = samples::findFile("./data/sample.jpg");
    std::string image_path = samples::findFile(fileName);
    std::cout << "Open image: !!\n";
    Mat img = imread(image_path, IMREAD_COLOR);
    if(img.empty())
    {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }
    imshow("Display window", img);
    int k = waitKey(0); // Wait for a keystroke in the window
    if(k == 's')
    {
        imwrite("starry_night.png", img);
    }
    return 0;
}