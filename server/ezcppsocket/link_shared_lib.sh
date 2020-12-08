#!/bin/bash
VERSION=".0.0.3"
# gcc -shared -fPIC -o libezcppsocket.so${VERSION} -I/usr/local/include/opencv4 ezcppsocket.cpp
ln -s $(pwd)/libezcppsocket.so${VERSION} /usr/local/lib/libezcppsocket.so
ldconfig