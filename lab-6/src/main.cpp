#include "CameraProvider.hpp"
#include "FrameProcessor.hpp"
#include "KeyProcessor.hpp"
#include "Display.hpp"
#include <iostream>

int main() {
    CameraProvider camera(0);
    if (!camera.isOpened()) {
        std::cerr << "Error: Camera not found! Check VMware settings." << std::endl;
        return -1;
    }

    FrameProcessor processor;
    KeyProcessor keys;
    std::string winName = "OpenCV Lab Work";
    Display display(winName);

    cv::createTrackbar("Brightness", winName, &FrameProcessor::brightnessValue, 100);

    std::cout << "Program started. Controls:" << std::endl;
    std::cout << "0-5: Change processing modes" << std::endl;
    std::cout << "ESC: Exit" << std::endl;

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) break;

        int key = cv::waitKey(30) & 0xFF;

        if (key == 27) break; // ESC

        if (key != 255) {
            keys.process(key);
            std::cout << "Key pressed: " << (char)key << " | Mode: " << (int)keys.getMode() << std::endl;
        }

        processor.process(frame, keys.getMode());
        display.show(frame);
    }

    return 0;
}
