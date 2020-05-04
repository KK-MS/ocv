/*
   Create a TCP socket
 */
#include<stdio.h>
#include <stdlib.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include <sys/types.h>
#include <ctype.h>
#include<io.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <fstream>
using namespace cv;
using namespace std;


#include <fcntl.h>
#include <errno.h>

#include "packet.h"
#include "netrx.h"

void detect_light(netrx* ptrclient, cv::Mat ROI)
{
	netrx cNliet;
	cv::Mat image_hsv;
	cv::Mat lower_hue_range;
	cv::Mat upper_hue_range;
	cv::Mat hue_image;
	cv::medianBlur(ROI, ROI, 3);
	cv::cvtColor(ROI, image_hsv, COLOR_BGR2HSV);
	int process_start_green = 2;
	int process_start_red = 1;

	cout << "Press 1 to detect red light" << endl << endl;
	cout << "Press 2 to detect Green light" << endl << endl;
	cin >> ptrclient->stPacket.red_color;


	if (ptrclient->stPacket.red_color == process_start_red)
	{
		cv::inRange(image_hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_hue_range);
		cv::inRange(image_hsv, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_hue_range);
	}

	if (ptrclient->stPacket.red_color == process_start_green)
	{

		cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(86, 255, 255), lower_hue_range);
		cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(70, 255, 255), upper_hue_range);
	}

	/*if (ptr->color >= 3)
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
	}*/

	cv::addWeighted(lower_hue_range, 1.0, upper_hue_range, 1.0, 0.0, hue_image);
	cv::GaussianBlur(hue_image, hue_image, cv::Size(9, 9), 2, 2);
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(hue_image, circles, HOUGH_GRADIENT, 1, hue_image.rows / 8, 100, 27, 0, 0);
	if (circles.size() == 0)
	{
		//std::cout << std::noboolalpha << P.output2 << '\n';
		std::exit(-1);
	}
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);
		//std::cout << std::noboolalpha << P.output1 << '\n';
		cv::circle(ROI, center, radius, cv::Scalar(139, 0, 0), 3);

		cv::namedWindow("Detected input image", cv::WINDOW_AUTOSIZE);
		cv::imshow("Detected input image", ROI);
		cv::namedWindow("Detected hsv image", cv::WINDOW_AUTOSIZE);
		cv::imshow("Detected hsv image", image_hsv);

	}

	return void();
}



int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in client;
	netrx cNliet;

	memset(&client, 0, sizeof(client));

	int send_size;
	int read_size;
	char info[] = "APP";
	Mat ROI;
	Mat img;
	Mat image_hsv;

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

	netrx ptrclient;

	// Packet structure define
	PACKET* ptr_metadata = (PACKET*) & (ptrclient.stPacket);

#if 0

		//send the data
		send_size = send(sock, info, sizeof(info), 0);

		if (send_size == -1) {
			printf("\n***Error in sending request for Lane information: [%d] : %s, [%d]\n", errno, strerror(errno), send_size);
			return -1;
		}

		printf("send Size: %d\n", send_size);
#endif

		
		
		while (1)
		{
             //  Receiving data from server

			//Processing of image

			char name[50];

			for (int i = 0; i <= 2; i++)
			{
				sprintf_s(name, "C:/images/image%d.png", i);
				Mat img = imread(name, 1);
				if (img.empty())
				{
					printf("image not loaded");
					break;
				}
				read_size = recv(sock, (char*)ptr_metadata, sizeof(PACKET), 0);
				if (read_size < 0) {
					printf("read_size:%d\n", read_size);
					printf("***Error in Receiving MetaData: [%d] : %s\n", errno, strerror(errno));
					return -1;
				}

				if (read_size != sizeof(PACKET)) {
					printf("***Error: RX METADATA PACKET got %d\n", read_size);
					// TODO: log the error and request again for metadata
				}

				printf("read_size:%d\n", read_size);
				printf("\nReceived ROI x_coordinate value: %d\n", ptr_metadata->x_coordinate);
				printf("\nReceived ROI y_coordinate value: %d\n", ptr_metadata->y_coordinate);
				printf("\nReceived ROI width: %d\n", ptr_metadata->width);
				printf("\nReceived ROI height: %d\n", ptr_metadata->height);

				Rect2i myROI(ptr_metadata->x_coordinate, ptr_metadata->y_coordinate, ptr_metadata->width, ptr_metadata->height);
				ROI = img(myROI);
				detect_light(&cNliet, ROI);
				ofstream myfile;
				myfile.open("output.txt");
				myfile << "Red or Green light is detected\n";
				myfile.close();
				waitKey(1);
			}

		}

	closesocket(sock);
	WSACleanup();
	return 0;
}

