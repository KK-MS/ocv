# GT Server_Standalone ( ZIM - 253 / (.exe) relesase)
Calculate the GT_D2L using IMU => Calculate the IMU (Calibrated point) D2L using GT

0. Read the config.json file to read the json strings as a command line arguments
1. Load the IMU_file and CRO_file 
2. Read the IMU & CRO data
3. Calculate the GT_D2L using IMU , CRO data & Bearing
3. Load the IMU Image to process the ODO_D2L
4. on the base of ODO_D2L relocalize the car postion
5. Check the confidence level after relocalize car position (TODO)
6. Saving the Odometry data in to csv fie for furthure Analysis or applciation usage

## Compilation
```console
Visual studio Compilation
```

## Run
From the Command prompt
```console
<gt_server_standalone> 

gt_server_standalone_searching.exe
```

## Output
saved process Relocalize data
```console
odometry.csv
``` 