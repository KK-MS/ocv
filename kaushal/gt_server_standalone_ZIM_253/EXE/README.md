# GT Server_Standalone ( ZIM - 253 / (.exe) relesase)
Calculate the GT_D2L using IMU => Calculate the IMU (Calibrated point) D2SL/D2TS using GT data

1. Read the config.json file to parse the json strings as a command line arguments to structure variables
2. Load the IMU_file and CRO_file 
3. Read the IMU & CRO data
4. Calculate the GT_D2L using IMU , CRO data & Bearing
5. Load the IMU Image to process the ODO_D2L
6. On the base of ODO_D2L relocalize the car postion (FR Wheel)
7. Car Relocalization to GPS Mounting (Mounting Offset)
8. Check the confidence level after relocalize car position
9. Saving the Odometry data in to csv fie for furthure Analysis or applciation usage

##information
```console
config.txt should be with gt_server_standalone_searching.exe fie
```

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