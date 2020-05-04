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
	bool output1 = true;
	bool output2 = false;

	//which color to detect
	int green_color;
	int red_color;
} PACKET;
#endif