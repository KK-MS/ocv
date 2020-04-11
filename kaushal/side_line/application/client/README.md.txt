# ODOMETRY Clinet
Receving the METADATA (Updated packet with new car position) from Host server.
Saving the data in .csv file for use in further Application / Analysis.

## Compilation
in MSYS-2017
```console
g++ TCP_client.cpp -o client -lws2_32
```

## Run

```console
./client
```