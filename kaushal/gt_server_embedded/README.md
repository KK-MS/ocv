# GT Server_Embedded
1. Connect with HOST Client
2. Receive the request form client, Read the GT_Data From CSV file 
3. Send data to Client for Prcocess the frame to Create the ROI > D2L / D2TS 

## Compilation
in MSYS
```console
g++ main.cpp network.cpp server_tcp.cpp -I ../include -o gt_server -lws2_32 -std=c++11
```

## Run

```console
./gt_server.exe 127.0.0.1 8889 ZIM212.csv
```