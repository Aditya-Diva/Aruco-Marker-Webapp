#include "run_server.h"

// Init Aruco server as global variable
Server g_aruco_server;

/**
 * @brief Checks for markers on the image
 * 
 * @param inputImage Image on which aruco markers are to be detected (reference)
 * @return cv::Vec3d Return single 3 element vector of first marker spotted
 */
std::vector<float> Server::detectMarker(cv::Mat inputImage)
{
    std::vector<float> result;

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

    cv::aruco::detectMarkers(inputImage, this->m_dictionary, markerCorners, markerIds, this->m_parameters, rejectedCandidates);

    cv::Mat distCoeffs = cv::Mat(1, 5, CV_32F, this->m_raw_dist_coeff);

    if (!markerIds.empty())
    {
        cv::aruco::drawDetectedMarkers(inputImage, markerCorners, markerIds);

        std::vector<cv::Vec3d> rvecs, tvecs;
        cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, this->m_camera_matrix, distCoeffs, rvecs, tvecs);
        // draw axis for each marker
        for (int i = 0; i < markerIds.size(); i++)
            cv::aruco::drawAxis(inputImage, this->m_camera_matrix, distCoeffs, rvecs[i], tvecs[i], 0.1);

        // Load the values one after the other
        for (int i = 0; i < 3; i++)
            result.push_back(float(tvecs[0][i]));
    }
    return result;
}

/**
 * @brief Create a My Marker object
 * Use this function to save a new marker image after collecting it from the dictionary
 */
void Server::createMyMarker()
{
    cv::Mat markerImage;
    cv::aruco::drawMarker(this->m_dictionary, this->m_marker_id, this->m_marker_size, markerImage, 1);

    int border_thickness;
    if (this->m_grid_dims != 0)
        border_thickness = int(this->m_marker_size / this->m_grid_dims);

    cv::copyMakeBorder(markerImage, markerImage, border_thickness, border_thickness, border_thickness, border_thickness, cv::BORDER_CONSTANT, cv::Scalar(255));
    cv::imwrite("marker" + std::to_string(this->m_marker_id) + ".png", markerImage);
    // cv::imshow("markerImage", markerImage);
    // cv::waitKey(0);
    // cv::destroyAllWindows();
}

/**
 * @brief Function to run for continous webcam stream processing
 * Press Esc to exit the stream
 * 
 * @return int 
 */
int Server::inferenceStart()
{

    cv::Mat frame;
    cv::VideoCapture cap;
    cap.open(this->m_device_id, cv::CAP_ANY);
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    // Webcam loop
    for (;;)
    {
        cap.read(frame);
        if (frame.empty())
        {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        // Perform aruco detection here
        this->detectMarker(frame);

        cv::imshow("Live", frame);
        if (cv::waitKey(1) == 27)
            break;
    }

    return 0;
}

/**
 * @brief Infer on a single frame
 * 
 * @param frame frame to be inferred upon
 * @return cv::Vec3d Return 3 elemment vector of marker centroid pose
 */
std::vector<float> Server::inferCurrentFrame(cv::Mat frame)
{
    if (frame.empty())
        std::cerr << "ERROR! blank frame grabbed\n";

    // Perform aruco detection here
    return this->detectMarker(frame);
}

/**
 * @brief Function that operaters on every server request
 * 
 * @param s EzCppSocket server instance
 */
void serverOperation(EzCppSocket &s)
{
    cv::Mat frame;
    //Receive image from client
    frame = s.readImage();
    std::vector<float> result;

    if (!frame.empty())
    {
        // Processing here
        result = ::g_aruco_server.inferCurrentFrame(frame);
        if (!result.empty())
        {
            for (auto elem : result)
                std::cout << elem << " ";
            std::cout << "\n";
        }
    }

    // Send processed image to client
    s.sendImage(frame);
    // Send coordinates value
    s.sendFloatList(result);
}

int main()
{
    // For cpp development
    // ::g_aruco_server.inferenceStart(); // Infer with cpp script
    // ::g_aruco_server.createMyMarker(); // Create a marker to test something on your own

    // To start ezcppserver
    while (true)
    {
        EzCppSocket s = EzCppSocket("127.0.0.1", 10000, 2, 1, false, true, 1, true, 5, std::pair<std::string, std::string>("start", "end"));
        try
        {
            s.serverLoop(&serverOperation);
        }
        catch (...)
        {
            std::cout << "Server was shut down. Possibly due to client disconnection. Restarting for other clients...\n";
        }
    }

    return 0;
}