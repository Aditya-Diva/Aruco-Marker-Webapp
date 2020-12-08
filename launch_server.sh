#!/bin/bash
docker run -it --rm --net=host -e DISPLAY=$DISPLAY \
-v "$(pwd)":/home/aruco-webapp -v /tmp/.X11-unix:/tmp/.X11-unix \
--user user --device=/dev/video0:/dev/video0 aruco-webapp:server