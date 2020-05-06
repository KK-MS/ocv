# GT Clinet
1. Connect with HOST server.
2. Read the GT_Data From CSV file and send it to server for Prcocess the frame.

## Compilation
in MSYS
```console
g++ main.cpp network.cpp server_tcp.cpp -I ../include -o gt_server -lws2_32 -std=c++11
```

## Run

```console
./gt_server.exe 127.0.0.1 8889 ZIM212.csv
```