import ctypes
import cv2
import time

if __name__ == "__main__":
    # TODO: Don't use absolute path here!
    c_lib = ctypes.CDLL("/home/adityad/projects/aruco_project/aruco-webapp/main.so")

    WEBCAM_MODE = False
    ret = True
    frame_count = 0
    if WEBCAM_MODE:
        cam = cv2.VideoCapture(0)

    start = time.time()
    while(ret):

        if WEBCAM_MODE:
            ret, image = cam.read()
        else:
            c_lib.inference_current_frame()
            image = cv2.imread("frame.jpg")

        frame_count += 1
        cv2.imshow("frame", image)
        print("FPS : ", frame_count/(time.time()-start))
        key = cv2.waitKey(1)
        if key == ord('q'):
            break
