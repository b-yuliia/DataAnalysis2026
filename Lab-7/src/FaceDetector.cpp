#include "FaceDetector.hpp"
#include <iostream>
#include <chrono>

FaceDetector::FaceDetector() : isRunning(true), hasNewFrame(false) {
    try {
        net = cv::dnn::readNetFromCaffe("deploy.prototxt", "res10_300x300_ssd_iter_140000.caffemodel");
        if (!net.empty()) {
            std::cout << ">>> SUCCESS: Face Detector model loaded successfully! <<<" << std::endl;
        }
    } catch (const cv::Exception& e) {
        std::cerr << "Error loading Face Detector model: " << e.what() << std::endl;
    }

    workerThread = std::thread(&FaceDetector::workerLoop, this);
}

FaceDetector::~FaceDetector() {
    isRunning = false;
    cv_Notification.notify_one();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void FaceDetector::updateFrame(const cv::Mat& frame) {
    if (frame.empty()) return;

    {
        std::lock_guard<std::mutex> lock(mtx);
        frame.copyTo(currentFrame);
        hasNewFrame = true;
    }
    cv_Notification.notify_one();
}

void FaceDetector::workerLoop() {
    while (isRunning) {
        cv::Mat frameToProcess;

        {
            std::unique_lock<std::mutex> lock(mtx);
            cv_Notification.wait(lock, [this]() { return hasNewFrame || !isRunning; });

            if (!isRunning) break;

            currentFrame.copyTo(frameToProcess);
            hasNewFrame = false;
        }

        if (net.empty() || frameToProcess.empty()) continue;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        cv::Mat blob = cv::dnn::blobFromImage(frameToProcess, 1.0, cv::Size(300, 300), 
                                             cv::Scalar(104.0, 177.0, 123.0), true, false);
        net.setInput(blob);
        cv::Mat detections = net.forward();

        cv::Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
        std::vector<std::pair<cv::Rect, std::string>> localDetections;

        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);

            if (confidence > 0.45f) {
                int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frameToProcess.cols);
                int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frameToProcess.rows);
                int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frameToProcess.cols);
                int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frameToProcess.rows);

                xLeftBottom = std::max(0, std::min(xLeftBottom, frameToProcess.cols - 1));
                yLeftBottom = std::max(0, std::min(yLeftBottom, frameToProcess.rows - 1));
                xRightTop = std::max(0, std::min(xRightTop, frameToProcess.cols - 1));
                yRightTop = std::max(0, std::min(yRightTop, frameToProcess.rows - 1));

                if (xRightTop - xLeftBottom > 10 && yRightTop - yLeftBottom > 10) {
                    cv::Rect rect(xLeftBottom, yLeftBottom, xRightTop - xLeftBottom, yRightTop - yLeftBottom);
                    std::string label = "Face: " + std::to_string(static_cast<int>(confidence * 100)) + "%";
                    localDetections.push_back({rect, label});
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            lastDetections = localDetections;
        }
    }
}

void FaceDetector::drawFaces(cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& detection : lastDetections) {
        cv::rectangle(frame, detection.first, cv::Scalar(0, 255, 0), 3);
        cv::putText(frame, detection.second, cv::Point(detection.first.x, detection.first.y - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
    }
}
