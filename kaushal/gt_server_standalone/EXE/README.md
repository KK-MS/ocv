# GT Server_Standalone
1. Load the IMU_file and CRO_file 
2. Read the IMU & CRO data
3. Calculate the GT_D2L using IMU , CRO data & Bearing
3. Load the IMU Image to process the ODO_D2L
4. on the base of ODO_D2L relocalize the car postion
5. Saving the Odometry data in to csv fie for furthure Analysis or applciation usage

## Compilation

```console
Visual studio Compilation
```

## Run

```console
<gt_server_standalone> <IMU_filename> <Mapped_CRO_filename> <IMU_Image_folder_path> <save_odometry_data_filename>

./gt_server_standalone.exe A7_IMU_24.04.20.csv A7_CRO_GT_24.04.20.csv D:/A7_measurement_24.4.20/evening/img/\*.png odometry.csv
```

## Output
saved process Relocalize data

```console
odometry.csv
```