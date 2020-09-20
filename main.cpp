#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <opencv2/aruco.hpp>


// Settings
auto g_dict = cv::aruco::DICT_6X6_250;
int g_grid_dims = 6;
int g_marker_id = 62;
int g_marker_size = 200;
// open the default camera using default API
int g_deviceID = 0; // 0 = open default camera

extern "C"{
void detectMarker(cv::Mat inputImage)
{
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    parameters->minDistanceToBorder = 0;
    parameters->adaptiveThreshWinSizeMax = 400;

    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(::g_dict);
    cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

    if (!markerIds.empty())
        cv::aruco::drawDetectedMarkers(inputImage, markerCorners, markerIds);
}

void createMyMarker()
{
    cv::Mat markerImage;
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(::g_dict);
    cv::aruco::drawMarker(dictionary, ::g_marker_id, ::g_marker_size, markerImage, 1);

    int border_thickness;
    if (::g_grid_dims != 0)
        border_thickness = int(::g_marker_size / ::g_grid_dims);

    cv::copyMakeBorder(markerImage, markerImage, border_thickness, border_thickness, border_thickness, border_thickness, cv::BORDER_CONSTANT, cv::Scalar(255));
    cv::imwrite("marker" + std::to_string(::g_marker_id) + ".png", markerImage);
    // cv::imshow("markerImage", markerImage);
    // cv::waitKey(0);
    // cv::destroyAllWindows();
}

int inference_start()
{

    cv::Mat frame;
    cv::VideoCapture cap;
    cap.open(::g_deviceID, cv::CAP_ANY);
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
        detectMarker(frame);

        cv::imshow("Live", frame);
        if (cv::waitKey(1) >= 0)
            break;
    }

    return 0;
}


void inference_current_frame()
{
    static cv::Mat frame;
    static cv::VideoCapture cap;
    cap.open(::g_deviceID, cv::CAP_ANY);
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
    }

    // Webcam frame
    cap.read(frame);
    if (frame.empty())
        std::cerr << "ERROR! blank frame grabbed\n";

    // Perform aruco detection here
    detectMarker(frame);
    cv::imwrite("./frame.jpg", frame);
    // cv::waitKey(0);
    // return frame;
}

void hello_world(){
    std::cout << "hello world!\n";
}

}
int main()
{
    // To createMarker
    // createMyMarker();

    // Start inferencing through webcam
    // return inference_start();

    inference_current_frame();
}