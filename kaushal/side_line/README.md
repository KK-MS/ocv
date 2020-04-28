# Localization setup

## IMU and Camera

APO is a carmaker client,
1. collects the IMU data from the CarMaker.
2. Capture Images from CarMaker Movie

## Location using Groundtruth (GT) and Visual Odometer
Host_server plays a role of odometer.
* Step-1: Odometry receives the input data (IMU + Images),
* Step-2: Odometry pass IMU data to Ground truth and get the Sidelane/Traffic sign details.
* Step-3: Odometry calculates the distance and angle between GT lat-long-bearing and IMU lat-long-bearing and with it defines Region of Interest based on their (IMU, GT) accuracy.
* Step-4: Odometry calculates the distance of the interested object/lane using image processing, also derives confident level.
* Step-5: Odometry calculates the current position using Step-4 distane and GT Lat-long.
* Step-6: Odometry provides the calculated value and its confident level to the application.

## Application
Demo application that recevies the repositioned lat-long


Current design:
1. Odometry run as a server
2. Odometry accepts client connection in order:
2.1. VSD/Metadata client
2.2. GT client
2.2. Application client


## File description / overview

1. netrx_tcp.cpp: has the main while loop

