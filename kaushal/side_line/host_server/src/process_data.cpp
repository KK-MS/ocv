/*****************************************************************************
 *
 * A Drive Living Lab Confidential Proprietary
 *
 * Copyright (c) 2019 A Drive Living lab
 * All Rights Reserved
 *
 *****************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY A DRIVE LIVING LAB "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL A DRIVE LIVING LAB OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
*****************************************************************************/

#include <iostream>
#include <stdio.h>

#include "opencv2/opencv.hpp"

// Application includes
#include "netrx.h"
#include "packet.h"

using namespace std;
using namespace cv;

/*
 ** process_data
 **
 ** Image Processing(D2L) on received frame.
 */
int process_data(netrx *ptrCliNet, char *ptr_frame_buf, int height, int width)
{
  PACKET *ptr_metadata = (PACKET *) &(ptrCliNet->stPacket);

  // Variables for getting the Pixel value
  Scalar pixelValue;
  uchar pixel;

  // Create the MAT objects
  Mat frame, edges, img_rgb, img_resize;
  Mat ROI, img_lane, img_lines, img_HSV;

  // Get the frame
  cv::Mat rawDataRight(height, width, CV_8UC3, (void*)ptr_frame_buf);
  
#if 1
  // change Mat frame from BGR to RGB
  cvtColor(rawDataRight, img_rgb, COLOR_BGR2RGB);
  
  //imshow("Raw_img", img_rgb);

  // Resize the image
  cv::Size umSize(WIDTH,HEIGHT);
  cv::resize(img_rgb, img_resize, umSize);
#endif
    
  // Convert the color frame to gray
  cvtColor(img_resize, edges, COLOR_BGR2GRAY);

#if 0
  // Create the Region Of Interest
  Rect const box(500, 0, 780, 720); // and the second corner is
									//(x + b, y+c )= (100 +400,295+185)
  ROI = img_resize(box);

  // convert our img to HSV Space
  cvtColor(ROI, img_HSV, COLOR_RGB2HSV);
//#if 0
  //white color thresholding
  Scalar whiteMinScalar = Scalar(0, 0, 168); // 0 0 0 // 0 0 168
  Scalar whiteMaxScalar = Scalar(50, 189, 255); // 50 189 255 // 172 111 255

  inRange(img_HSV, whiteMinScalar, whiteMaxScalar, img_lane);
//#if 0
  // Edge detection using canny detectionector
  int minCannyThreshold = 190;
  int maxCannyThreshold = 230;
  Canny(img_lane, img_lines, minCannyThreshold, maxCannyThreshold); // , 3, true);
//#if 0
  // Morphological Operation
  Mat k = getStructuringElement(MORPH_RECT, Size(9, 9)); //MACROSTLAB :k=Ones(9)

  morphologyEx(img_lines, img_lines, MORPH_CLOSE, k);
//#if 0
  // now applying hough transform TO dETECT Lines in our image
  vector<Vec4i> lines; // For storage the detected line values

  //HoughLinesP(LinesImg, lines, rho, theta, threshold, minLinLength, maxLineGap);
  HoughLinesP(img_lines, lines, 1, CV_PI / 180, 150, 0, 50);

  //draw our lines
  for (size_t i = 0; i < lines.size(); i++) {

	  Vec4i l = lines[i];  // we have 4 elements p1=x1,y1  p2= x2,y2
	  Scalar greenColor = Scalar(0, 250, 30);  // B=0 G=250 R=30
	  line(ROI, Point(l[0], l[1]), Point(l[2], l[3]), greenColor, 3, 4);
  }

  // applying Canny edge detection of lined drawn image frame
  Canny(img_resize, edges, 250, 255, 3, true);

#endif

#if 1

  for (int i = ptrCliNet->istart_x; i < FRAME_WIDTH; i++) {

    pixel = (uchar)edges.at<uchar>(Point(i, ptrCliNet->iend_y));

    if (pixel >= ptrCliNet->iLaneColorUpperThreshold) {
      ptrCliNet->iLanePixelCount++;
    }
    else {
      ptrCliNet->iLanePixelCount = 0;
    }

    //printf("x:%d, y:%d , val= %u \n", i, ptrCliNet->iend_y, pixel);

    if (ptrCliNet->iLanePixelCount > ptrCliNet->iLanePixelThreshold) {

      // Find the D2L
      i -= ptrCliNet->iLanePixelThreshold;

      // Taking the Paythagoras theorem to find the distance(Diagonal)
      Point2f a(ptrCliNet->istart_x, ptrCliNet->istart_y);
      Point2f b(i, ptrCliNet->iend_y);

      int result = cv::norm(cv::Mat(b), cv::Mat(a));
      
      float distance = result * 3.0 * 0.001;

      ptr_metadata->u4_odo_distance = distance;

      printf("\nx:%d, y:%d , val= %u \n", i, ptrCliNet->iend_y, pixel);
      printf("We got the lane, found D2L= %f m\n", distance);

      // Display the Frames
      line(img_resize, Point(ptrCliNet->istart_x, ptrCliNet->istart_y)
          , Point(i, ptrCliNet->iend_y), Scalar(255, 255, 0), 1, 8);

      // Put the text on frame for information
      putText(img_resize, format("D2L: %f m", distance)
          , Point(550, ptrCliNet->iend_y - 20)
          , FONT_HERSHEY_PLAIN
          , 1, Scalar(0, 255, 0));

      // Put the text on frame for information
      putText(img_resize, format("TS: %f, Lat: %f, Lon: %f,CM_D2L: %f", ptr_metadata->u4_timestampL, ptr_metadata->u4_ins_latitude, ptr_metadata->u4_ins_longitude, ptr_metadata->u4_ins_cm_d2l)
          , Point(700, 600)
          , FONT_HERSHEY_PLAIN
          , 1, Scalar(255, 255, 0));
      // Put the logo in main frame
      ptrCliNet->mat_logo.copyTo(img_resize(cv::Rect(1120,650,
              ptrCliNet->mat_logo.cols, ptrCliNet->mat_logo.rows)));

      // Write to video file
      //ptrCliNet->wrOutVideo.write(img_resize);

	  imshow("Odo_D2L", img_resize);
	  
	  printf("Process frame Displayed.\n");

	  waitKey(100);
      break;
    }
  }

#endif
  
  return 0;
}
