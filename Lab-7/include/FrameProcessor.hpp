#ifndef FRAME_PROCESSOR_HPP
#define FRAME_PROCESSOR_HPP

#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"
#include "FaceDetector.hpp"

class FrameProcessor {
public:
    FrameProcessor();
    void process(cv::Mat& frame, ProcessMode mode);
    
    static int brightnessValue;

private:
    FaceDetector faceDetector;
};

#endif
