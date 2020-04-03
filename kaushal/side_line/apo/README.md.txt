# APO Application
Side line application.
Receiving the CM METADAT and FRAMEDATA then send it to host server for image Processing and Correction of car position.

## Compilation
in MSYS-2017
```console
make
```

## Run

```console
./ApoClntDemo.win64 <CM_IP> <Server_IP> <Server_port> <-v>

For ex:
./ApoClntDemo.win64 <10.242.6.8> <127.0.0.1> <8889> <-v>
```