
# Process

1. It receives the input data (IMU + Images)
2. Odometry pass IMU data to Ground truth and get the Sidelane/Traffic sign details
3. Odometry calculates the Region of Interst from IMU Lat-Long-bearing and GT Lat-long
4. Odometry calculates teh distance of the intersted object/lane
5. Odometry calculates the current position from Distane and GT Lat-long
6. Odometry passt the calculated value to the application


## As per work done pending process

1. Done
2. Done
3. currently, It is done in step - 4 
4. Done
5. Currently, it is already done in step - 2 GT_client side. Later stage will do in this step host side.
6. Done

# Work Done

1. Create the Tool chain for receving the CarMaker METADATA and FRAMAEDAT
2. Send it to GT_Python client for coorection of GPS Position.
3. Recive the Corrected GPS Car position.
4. process the Frame to find the D2L.
5. send the Updated packet to Applciation client.
6. Applciation client recieve the Updated METADATA and save it in CSV file to use it in furthur application.



