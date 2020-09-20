#!/bin/bash
g++ main.cpp -o main `pkg-config --cflags --libs opencv4`
./main

# g++ -shared -o main.so -fPIC  main.cpp `pkg-config --cflags --libs opencv4`