#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/aruco.hpp>

#include "ezcppsocket.h"

#ifndef __ARUCO_SERVER__
#define __ARUCO_SERVER__

class Server
{
    // Class Variables

    // Aruco Detection Settings
    cv::aruco::PREDEFINED_DICTIONARY_NAME m_dict = cv::aruco::DICT_6X6_250;
    cv::Ptr<cv::aruco::Dictionary> m_dictionary = cv::aruco::getPredefinedDictionary(m_dict);

    // To save a marker locally (createMyMarker() function)
    int m_grid_dims = 6;
    int m_marker_id = 62;
    int m_marker_size = 200;

    // Setting Detector Parameters
    cv::Ptr<cv::aruco::DetectorParameters> m_parameters = cv::aruco::DetectorParameters::create();

    // Camera settings
    // Open the default camera using default API
    int m_device_id = 0; // 0 = open default camera
    cv::Mat m_camera_matrix = (cv::Mat_<double>(3, 3) << 6.5746697944293521e+002, 0., 3.1950000000000000e+002,
                               0., 6.5746697944293521e+002, 2.3950000000000000e+002,
                               0., 0., 1.);

    float m_raw_dist_coeff[5] = {-4.1802327176423804e-001, 5.0715244063187526e-001, 0., 0., -5.7843597214487474e-001};

public:

    // Constructor
    Server()
    {
        this->m_parameters->minDistanceToBorder = 0;
        this->m_parameters->adaptiveThreshWinSizeMax = 400;
    }

    // Class Methods
    std::vector<float> detectMarker(cv::Mat inputImage);
    void createMyMarker();
    int inferenceStart();
    std::vector<float> inferCurrentFrame(cv::Mat frame);

};

#endif