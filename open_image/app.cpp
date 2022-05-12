/*
 * Main function of the application
 */

#include <iostream>

// App specific
#include <app.h>

int main(int argc, char **argv)
{
    std::cout << "Hello World!\n";
    if (argc > 1) {
    //open_image(argv[1]);
    open_uyvy("C:/prj/ocv/open_image/video-18-6-9_0.uyvy", 1280, 720);
    //open_uyvy("C:/prj/ocv/open_image/sample_640x480.uyvy", 640, 480);
    
    //open_uyvy("C:/Users/Krishnakumar.Mayanna/Downloads/video-18-6-9_0.uyvy");
    }
}