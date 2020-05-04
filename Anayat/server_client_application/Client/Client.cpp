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

SOCKET sock;
Mat ROI;
Mat img;
int process_start_red = 1;
int process_start_green = 2;
char info[] = "A";
char info1[] = "BBB";
std::vector<cv::Vec3f> circles;

int create_socket(netrx* ptrclient)
{
	WSADATA wsa;
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

	ptrclient->sock_desc_app = sock;
	puts("\nConnected.");
	fflush(stdout);

	return 0;
}

int recv_packet(netrx* ptrclient, cv::Mat img)
{
	netrx cNliet;
	int read_size;
	PACKET* ptr_metadata = (PACKET*) & (ptrclient->stPacket);
	read_size = recv(ptrclient->sock_desc_app, (char*)ptr_metadata, sizeof(PACKET), 0);
	if (read_size < 0) {
		printf("read_size:%d\n", read_size);
		printf("***Error in Receiving MetaData: [%d] : %s\n", errno, strerror(errno));
		return -1;
	}

	if (read_size != sizeof(PACKET)) {
	        printf("***Error: RX METADATA PACKET got %d\n", read_size);
	}

	printf("read_size:%d\n", read_size);
	printf("\nReceived ROI x_coordinate value: %d\n", ptr_metadata->x_coordinate);
	printf("\nReceived ROI y_coordinate value: %d\n", ptr_metadata->y_coordinate);
	printf("\nReceived ROI width: %d\n", ptr_metadata->width);
	printf("\nReceived ROI height: %d\n", ptr_metadata->height);

	Rect2i myROI(ptr_metadata->x_coordinate, ptr_metadata->y_coordinate, ptr_metadata->width, ptr_metadata->height);
	ROI = img(myROI);

	return 0;
}

int detect_sign(netrx* ptrclient, cv::Mat ROI)
{
	netrx cNliet;
	PACKET stPacket;
	cv::Mat image_hsv;
	cv::Mat lower_hue_range;
	cv::Mat upper_hue_range;
	cv::Mat hue_image;
        cv::medianBlur(ROI, ROI, 3);
	cv::cvtColor(ROI, image_hsv, COLOR_BGR2HSV);

	cout << "Press 1 to detect red light" << endl << endl;
	cout << "Press 2 to detect Green light" << endl << endl;
	cin >> ptrclient->stPacket.color;


	if (ptrclient->stPacket.color == process_start_red){
		cv::inRange(image_hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_hue_range);
		cv::inRange(image_hsv, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_hue_range);
	}

	if (ptrclient->stPacket.color == process_start_green){
        cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(86, 255, 255), lower_hue_range);
		cv::inRange(image_hsv, cv::Scalar(40, 40, 40), cv::Scalar(70, 255, 255), upper_hue_range);
	}

	if (ptrclient->stPacket.color >= 3){
		printf("Invalid color selection\n");
		return 0;
	}
	if (ptrclient->stPacket.color <= 0){
		printf("Invalid color selection\n");
		return 0;
	}

	cv::addWeighted(lower_hue_range, 1.0, upper_hue_range, 1.0, 0.0, hue_image);
	cv::GaussianBlur(hue_image, hue_image, cv::Size(9, 9), 2, 2);
	cv::HoughCircles(hue_image, circles, HOUGH_GRADIENT, 1, hue_image.rows / 8, 100, 27, 0, 0);
	if (circles.size() == 0){
		//std::exit(-1);
		printf("Invalid color selection\n");
		return 0;
	}
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);
		cv::circle(ROI, center, radius, cv::Scalar(139, 0, 0), 3);
		printf("sent information: %s\n", info);
		cv::namedWindow("Detected input image", cv::WINDOW_AUTOSIZE);
		cv::imshow("Detected input image", ROI);
		cv::namedWindow("Detected hsv image", cv::WINDOW_AUTOSIZE);
		cv::imshow("Detected hsv image", image_hsv);
    }

	return 0;
}

int send_result(netrx* ptrclient)
{
	netrx cNliet;
	PACKET stPacket;
	int send_size;

	//send the data
	if (circles.size() >= 1) {
		send_size = send(sock, info, sizeof(info), 0);

		if (send_size == -1) {
			printf("\n***Error in sending request for Lane information: [%d] : %s, [%d]\n", errno, strerror(errno), send_size);
			return -1;
		}

		printf("send Size: %d\n", send_size);

	}

	if (circles.size() == 0) {
		send_size = send(sock, info1, sizeof(info1), 0);

		if (send_size == -1) {
			printf("\n***Error in sending request for Lane information: [%d] : %s, [%d]\n", errno, strerror(errno), send_size);
			return -1;
		}

		printf("send Size: %d\n", send_size);

	}

}

int main(int argc, char* argv[])
{
    PACKET stPacket;
    netrx cliNet;
    //Create Socket
    create_socket(&cliNet);
    
	while (1){

	   //Processing of image
           char name[50];
           for (int i = 0; i <= 2; i++){
		sprintf_s(name, "C:/images/image%d.png", i);
		Mat img = imread(name, 1);
		if (img.empty()){
		    printf("image not loaded");
		    break;
		}
		//Receive metadata from server
		recv_packet(&cliNet, img);
            
		//Process the image
		detect_sign(&cliNet, ROI);

		//Send the result
		send_result(&cliNet);
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


