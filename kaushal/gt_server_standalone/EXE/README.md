# GT Server_Standalone
Calculate the GT_D2L using IMU => Calculate the IMU (Calibrated point) D2L using GT
ZIM-213: IMU: ADMA G-Pro + GeneSys

1. Load the IMU_file and CRO_file 
2. Read the IMU & CRO data
3. Calculate the GT_D2L using IMU , CRO data & Bearing
3. Load the IMU Image to process the ODO_D2L
4. On the base of ODO_D2L relocalize the car postion (FR Wheel)
5. Car Relocalization to GPS Mounting (Mounting Offset)
6. Check the confidence level after relocalize car position
7. Saving the Odometry data in to csv fie for furthure Analysis or applciation usage

## Compilation
```console
Visual studio Compilation
```

## Run
From the Command prompt
```console
<gt_server_standalone> <IMU_filename> <Mapped_CRO_filename> <IMU_Image_folder_path> <save_odometry_data_filename> <lat_gps_mo_FR> <lon_gps_mo_FR>

gt_server_standalone.exe A7_IMU_7&8_24.04.20.csv A7_mapped_CRO_7&8_24.04.20.csv C:/Users/ll_stkapate/Desktop/Messungen_GeneSys/7&8/img/ odometry_7&8.csv -0.947 2.314
```

## Output
saved process Relocalize data
```console
odometry.csv
``` 