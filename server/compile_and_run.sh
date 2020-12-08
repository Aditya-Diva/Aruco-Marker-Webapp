#!/bin/bash
g++ -Iezcppsocket run_server.cpp -o run_server `pkg-config --cflags --libs opencv4` -lezcppsocket
./run_server

# g++ -shared -o main.so -fPIC  main.cpp `pkg-config --cflags --libs opencv4`