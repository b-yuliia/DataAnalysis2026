#include "FrameProcessor.hpp"

int FrameProcessor::brightnessValue = 50;

FrameProcessor::FrameProcessor() {}

void FrameProcessor::process(cv::Mat& frame, ProcessMode mode) {
    if (frame.empty()) return;

    frame.convertTo(frame, -1, 1, FrameProcessor::brightnessValue - 50);

    switch (mode) {
        case ProcessMode::INVERT:
            cv::bitwise_not(frame, frame);
            break;
        case ProcessMode::GRAY:
            cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
            break;
        case ProcessMode::CANNY: {
            cv::Mat gray, edges;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            cv::Canny(gray, edges, 100, 200);
            cv::cvtColor(edges, frame, cv::COLOR_GRAY2BGR);
            break;
        }
        case ProcessMode::BLUR:
            cv::GaussianBlur(frame, frame, cv::Size(15, 15), 0);
            break;
        case ProcessMode::GLITCH: {
            std::vector<cv::Mat> channels;
            cv::split(frame, channels);
            cv::Mat redChannel = channels[2].clone();
            int shift = 20;
            channels[2] = cv::Mat::zeros(redChannel.size(), redChannel.type());
            redChannel(cv::Rect(0, 0, redChannel.cols - shift, redChannel.rows))
                .copyTo(channels[2](cv::Rect(shift, 0, redChannel.cols - shift, redChannel.rows)));
            cv::merge(channels, frame);
            break;
        }
        default: break;
    }

    cv::putText(frame, "Keys: 0-5 | ESC: Exit", cv::Point(20, 40), 
                cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 255), 2);
}
