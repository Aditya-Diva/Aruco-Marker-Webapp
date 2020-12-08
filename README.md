# Aruco Marker Webapp

<!-- PROJECT LOGO -->
<br />
  <p align="center">
    Aruco Marker Web Application
    <br />
    <a href="#about-the-project">View Demo</a>
    ·
    <a href="https://github.com/Aditya-Diva/Aruco-Marker-Webapp/issues">Report Bug</a>
    ·
    <a href="https://github.com/Aditya-Diva/Aruco-Marker-Webapp/issues">Request Feature</a>
  </p>


## Table of Contents

* [About the Project](#about-the-project)
  * [Built With](#built-with)
* [Getting Started](#getting-started)
  * [Installation](#installation)
* [Demo](#demo)
  * [Run Examples](#run-examples)
  * [Processed Webcam Stream](#processed-webcam-stream)
  * [Docker](#docker)
  * [Local Installation / VirtualEnv](#local-installation-/-virtualenv)
* [Usage](#usage)
* [Contributing](#contributing)
* [License](#license)
* [Contact](#contact)

## About The Project

> Aruco Marker Web Application to visualize pose/detection and movement variation of Aruco Markers on a real-time dashboard. The dashboard(s) include stream play/pause option which halts/resumes streams on all dashboards open.

![About The Project Gif](imgs/short_demo.gif)

The following configuration is used:

| Server | Client |
|  :---: | :---:  |
|  Cpp   |   Py   |

Functionalities overview:

Client :
* Collects image from webcam
* Sends image to server
* Receive Aruco Marked image and Centroid Location information
* Updates Dashboard and keeps track of Play/Pause and Dashboard state

Server :
* Find Aruco Markers present in received image and superimpose
* Collect centroid of the first marker detected at any instance
* Relay information back to client

General :
* Tested on single system setup (Server & Client)
* Works with multiple tabs open in multiple browsers
* Added functionality for Play and Pause stream
* Play and Pause functionality works by stopping and restarting client, hence client processes halt when paused (process will not be polling away in the background)
* Clients are killed when the last dashboard is exited and starts back up when accessed through dashboard again

### Built With

* Python
* Cpp
* Docker
* OpenCV
* [Ez-Cpp-Python-Socket](https://github.com/Aditya-Diva/Ez-Cpp-Python-Socket)
* Flask
* Flask-SocketIO
* Docker Compose

## Getting Started

Preferably use dockers or virtual environments.
For a quickstart demo, do not follow local installation steps.

### Installation

1. Clone the repo
    ```sh
    git clone https://github.com/Aditya-Diva/Aruco-Marker-Webapp.git
    ```
2. Enter the directory of the repo

3. Follow build instruction
    * Docker (Linux)
        ``` sh
        ./build_docker.sh # Build docker image in demo or dev mode
        ```

## Demo
![demo_gif](imgs/demo.gif)

### Docker Compose

```sh
# This is under the prerequisite that demo mode was selected while building

docker-compose up # Launch server and client together
```

## Development
### Docker

```sh
# This is under the prerequisite that dev mode was selected while building
# Run the following commands in two different terminals
./launch_server.sh # Loads into server files directly 
./launch_client.sh # Loads into client files directly

# Server-dev
./compile_and_run.sh

# Client-dev
python3 run_client.py

# Finally to remove the docker images after playing around
./remove_docker.sh # Removes docker image from system
```

## Usage

### Camera Config
This is an optional step for those who just want to run the demo and are not worried about accuracy. \
To configure the camera specs, go to [run_server.h](server/run_server.h) and edit as necessary.
(By default this uses /dev/video0 (in-built webcam / first video source) to run.)

### Preparing Aruco Marker
To test out the web application for yourself, a marker needs to be shown to the camera feed. This could be loaded on a screen or printed out and held flat infront of the camera.

Marker Resources :
* Go to this [site](https://chev.me/arucogen/). Choose the 6x6 standard dictionary and use an ID of your liking.

  OR

* Run server docker in dev mode. Edit [run_server.h](server/run_server.h) to set id of the marker required. Comment out all code in main of [run_server.cpp](server/run_server.cpp) apart from the createMyMarker method.
  ````sh
  ./compile_and_run.sh
  ````
  You will now have marker\<marker_id\>.png in the [server](server) folder.

### Heads-up
* If the stream ever does get stuck, try to Pause and Play.

* To understand process flow, \
Refer to run_server and run_client scripts in [server](server/) & [client](client/) folders.

## Contributing

Any contributions made are greatly appreciated.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See [`LICENSE`](LICENSE) for more information.

## Contact

Aditya Divakaran - [@LinkedIn](https://www.linkedin.com/in/aditya-divakaran/) - [@Github](https://github.com/Aditya-Diva) - [@GMail](adi.develops@gmail.com)

Note:

* This was tested on Ubuntu 20.04 with Dockers.