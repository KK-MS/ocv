# ODOMETRY Clinet
Receving the METADATA (Updated packet with new car position) from Host server.
Saving the data in .csv file for use in further Application / Analysis.

## Compilation
in MSYS-2017
```console
g++ GT_Client.cpp  -o gt_client -lws2_32 -std=c++11
```

## Run

```console
./gt_client
```