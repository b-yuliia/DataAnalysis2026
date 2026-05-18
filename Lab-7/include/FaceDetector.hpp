#ifndef FACE_DETECTOR_HPP
#define FACE_DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

class FaceDetector {
public:
    FaceDetector();
    ~FaceDetector();

    void updateFrame(const cv::Mat& frame);
    
    void drawFaces(cv::Mat& frame);

private:
    void workerLoop();

    cv::dnn::Net net;
    
    std::thread workerThread;
    std::mutex mtx;
    std::condition_variable cv_Notification;
    std::atomic<bool> isRunning;
    std::atomic<bool> hasNewFrame;

    cv::Mat currentFrame;
    std::vector<std::pair<cv::Rect, std::string>> lastDetections;
};

#endif
