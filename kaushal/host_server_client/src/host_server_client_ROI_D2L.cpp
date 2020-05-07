#include <iostream>
#include <stdio.h>
#include <fstream>
#include <errno.h>

#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#pragma warning(disable:4996)

#include "opencv2/opencv.hpp"
#include "packet.h"
#include "netrx.h"

using namespace std;
using namespace cv;

#define START_X 243      // 485 
#define START_Y 260      // 380 
#define END_Y  START_Y   // 380
#define FRAME_WIDTH 640  // 1280 

#define PIXEL_DIST 2.27  // in to mm  by calibration  

/*
 ** Description
 **
 ** Host sever application to recieve the GT_data from GT_Client.
 ** For process the frame with create the ROI for Find D2L.
 */

int main(int argc, char* argv[])
{
	/*************************** NETWORK CREATE & CONNECT **********************/

	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in client;

	memset(&client, 0, sizeof(client));

	printf("\nInitialising Winsock...");

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");
	fflush(stdout);

	//Create a socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");
	fflush(stdout);

	// assign the address to scoket
	client.sin_addr.s_addr = inet_addr("127.0.0.1");
	client.sin_family = AF_INET;
	client.sin_port = htons(8889);

	//Connect to remote server
	if (connect(sock, (struct sockaddr*) & client, sizeof(client)) < 0)
	{
		puts("connect error");
		fflush(stdout);
		return 1;
	}

	puts("\nConnected.");
	fflush(stdout);

	// Strcuture define
	netrx ptrCliNet;

	/***************** RECIEVE THE GT_DATA & PROCESS THE FRAME ****************/

	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptrCliNet.stPacket);

	int send_size;
	int read_size;
	int iFrameCount = 0;
	int i = 0;
	FILE* file;

	// Parameters for ROI based on GT data 
	int GTD2L;  // in mm
	int pix_lane;
	int GT_point_lane_x;
	int GT_point_lane_y;
	int ROI_x1;
	int ROI_y1;
	int ROI_x2;
	int ROI_y2;

	// varibles fro GT_METADA csv file reading for create the ROI
	int row_count = 0;
	string row;
	bool Skip = true;

	// Variables for getting the Pixel value
	Scalar pixelValue;
	uchar pixel;
	
	// lane threshold parameters 
	int iLanePixelCount = 0;
	int iLanePixelThreshold = 10;
	int iLaneColorUpperThreshold = 250;
	
	Mat frame, resize_frame, edges, adrive_logo, new_logo;
	Mat whiteLane, LinesImg, HSV_Img;

	// Load the image files from folder in sequence
	vector<cv::String> fn;
	glob("D:/A7_measurement_14.4.20/ZIM212/*.png", fn, false); // ZIM 212
	
															   // to save the frames as video
	VideoWriter wrOutVideo;
	const String name = "./A7_south_processing.avi";

	wrOutVideo.open(name, VideoWriter::fourcc('M', 'J', 'P', 'G'), 10.0, Size(640, 480), true);
	
	// Client -> VD_Server: Request for Metadata.
	printf("\nRequest METADATA\n");
	ptr_metadata->u4_request_type = REQ_GT_LANE_INFO;

	send_size = send(sock, (char*)ptr_metadata, sizeof(PACKET), 0);
	if (send_size == -1) {
		printf("***Error in sending Request for MetaData: [%d] : %s\n", errno, strerror(errno));
		return -1;
	}

	printf("send Size: %d\n", send_size);

	// Process the frames to find D2L and save the processed frames in device
	while (1) {

		// open the file
		file = fopen("A7_south_process.csv", "a");

		// Host server recieves GT_Info from GT_Client
		read_size = recv(sock, (char*)ptr_metadata, sizeof(PACKET), 0);
		if (read_size < 0) {
			printf("***Error in Receiving MetaData: [%d] : %s\n", errno, strerror(errno));
			return -1;
		}

		if (read_size != sizeof(PACKET)) {
			printf("***Error: RX METADATA PACKET got %d\n", read_size);
			// TODO: log the error and request again for metadata
		}

		printf("read_size:%d\n", read_size);

		// Display GT_D2L Data
		printf("GT_D2L: %f cm\n", ptr_metadata->u4_ins_cm_d2l);

		// to create the filenames in sequence for save the process frames
		std::stringstream ss;
		ss << "D:/A7_measurement_14.4.20/img_process/process_2/" << i << ".png";

		// Load the frame to process
		frame = imread(fn[iFrameCount].c_str(), IMREAD_COLOR);

		printf("\nfile: %s", fn[iFrameCount].c_str());

		// If the frame is empty, break immediately
		if (frame.empty()) break;

		imshow("img", frame);

		// Convert the GT_D2L in mm
		GTD2L = ptr_metadata->u4_ins_cm_d2l * 10; // convert in mm to get the pixels

		// Convert the D2L in to pixels
		pix_lane = GTD2L / PIXEL_DIST;

		// GT point on Lnae (x,y)
		GT_point_lane_x = START_X + pix_lane;
		GT_point_lane_y = START_Y;

		// Create the points (x1, y1, x2, Y2) fro ROI with GPS_accuracy from GT +/- 4 cm
		ROI_x1 = GT_point_lane_x - 20;
		ROI_y1 = GT_point_lane_y - 20;
		ROI_x2 = 40;
		ROI_y2 = 40;

		cout << endl << "GT_D2L: " << GTD2L << " mm," << " Pix_ROI:" << pix_lane;

		printf("\nROI: X1= % d, Y1= % d, x2= % d,Y2= % d", ROI_x1, ROI_y1, ROI_x2, ROI_y2);

		// Here we define our region of interest
		Rect const box(ROI_x1, ROI_y1, ROI_x2, ROI_y2);

		// Create the ROI on main frame
		Mat ROI = frame(box);

		// Convert our fame to HSV Space for white color filter
		cvtColor(ROI, HSV_Img, COLOR_BGR2HSV);

		imshow("img_HSV", HSV_Img);

		// white color thresholding
		Scalar whiteMinScalar = Scalar(100, 100, 50);
		Scalar whiteMaxScalar = Scalar(255, 189, 255);

		// Add the white color pixels in to ROI
		inRange(ROI, whiteMinScalar, whiteMaxScalar, LinesImg);

		imshow("InRange", LinesImg);

		// Edge detection using canny detector
		int minCannyThreshold = 190; // 190
		int maxCannyThreshold = 230; // 230
		Canny(LinesImg, LinesImg, minCannyThreshold, maxCannyThreshold, 3, true);

		// Morphological Operation
		Mat k = getStructuringElement(MORPH_RECT, Size(5, 5)); //MATLAB :k=Ones(9) // process_1 

		morphologyEx(LinesImg, LinesImg, MORPH_CLOSE, k);

		// Now, applying Hough line transform to detect the Lines in our frame
		vector<Vec4i> lines;

		HoughLinesP(LinesImg, lines, 1, CV_PI / 180, 5, 0, 10); // process_2

																// Draw our lines
		for (size_t i = 0; i < lines.size(); i++) {

			Vec4i l = lines[i];  // we have 4 elements p1=x1,y1  p2= x2,y2
			Scalar greenColor = Scalar(0, 255, 0);  // B=0 G=250 R=30
			line(ROI, Point(l[0], l[1]), Point(l[2], l[3]), greenColor, 1.5, 4);
		}

		Canny(frame, LinesImg, 250, 255, 3, true);

		imshow("canny 2", LinesImg);

		// process D2L
		for (int i = START_X; i < FRAME_WIDTH; i++) {

			pixel = (uchar)LinesImg.at<uchar>(Point(i, END_Y));

			if (pixel >= iLaneColorUpperThreshold) {
				iLanePixelCount++;
			}
			else {
				iLanePixelCount = 0;
			}

			//printf("x:%d, y:%d , val= %u \n", i, END_Y, pixel);

			if (pixel >= iLaneColorUpperThreshold) {

				Point2f a(START_X, START_Y);
				Point2f b(i, END_Y);

				int result = cv::norm(cv::Mat(b), cv::Mat(a));

				float distance = result * PIXEL_DIST * 0.1;

				printf("\nx:%d, y:%d , val= %u \n", i, END_Y, pixel);
				printf("We got the lane, found D2L= %f cm\n", distance);

				// Display the Frames
				line(frame, Point(START_X, START_Y), Point(i, END_Y), Scalar(255, 255, 0), 1, 8);

				// put the text Frame number on real frame for information
				putText(frame, format("Frame: %s", fn[iFrameCount].c_str()), Point(0, 450), FONT_HERSHEY_PLAIN, 0.75, Scalar(255, 255, 255));

				// put the text D2L on Real frame for information
				putText(frame, format(" D2L: %f cm", distance), Point(0, 400), FONT_HERSHEY_PLAIN, 1.5, Scalar(255, 255, 255));

				// add the Adrive logo
				adrive_logo = imread("ADrive_Logo1.png", IMREAD_COLOR);

				// Put the LOGO on resize frame
				cv::resize(adrive_logo, new_logo, Size(135, 50));

				// Put the LOGO on real frame
				new_logo.copyTo(frame(cv::Rect(500, 420, new_logo.cols, new_logo.rows)));

				// Write the process METADATA in .csv file
				//fprintf(file, "%s, %f\n", fn[iFrameCount].c_str(), distance);

				// save process_frame as image
				//imwrite(ss.str().c_str(), frame);

				// Write to video file
				//wrOutVideo.write(frame);

				// Display the original frame or process frame
				imshow("Process_frame_D2L", frame);

				break;
			}
		}

		fclose(file);

		if (waitKey(0) == 27)
		{
			cout << "esc";
			break;
		}

		iFrameCount++;
		i++;
	} // loop

	// Closes all the frames
	destroyAllWindows();

	return 0;
}