#ifndef PACKET_H
#define PACKET_H
// Main request packet
typedef struct packet {
	//ROI Parameters
	int height;
	int width;
	int x_coordinate;
	int y_coordinate;

	//output as a flag
	int output1;
	int output2;

	//which color to detect
	int color;

} PACKET;
#endif