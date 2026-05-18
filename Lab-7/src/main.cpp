#include "CameraProvider.hpp"
#include "FrameProcessor.hpp"
#include "KeyProcessor.hpp"
#include "Display.hpp"
#include <iostream>
#include <chrono>

int main() {
    CameraProvider camera(0);
    if (!camera.isOpened()) {
        std::cerr << "Error: Camera not found! Check VMware settings." << std::endl;
        return -1;
    }

    FrameProcessor processor;
    KeyProcessor keys;
    std::string winName = "OpenCV CV/ML Lab";
    Display display(winName);

    cv::createTrackbar("Brightness", winName, &FrameProcessor::brightnessValue, 100);

    std::cout << "Program started. Controls:" << std::endl;
    std::cout << "0-5: Classic filters" << std::endl;
    std::cout << "F: Face Detection mode" << std::endl;
    std::cout << "ESC: Exit" << std::endl;

    while (true) {
        auto start = std::chrono::high_resolution_clock::now();

        cv::Mat frame = camera.getFrame();
        if (frame.empty()) break;

        int key = cv::waitKey(1) & 0xFF;
        if (key == 27) break; 

        if (key != 255) {
            keys.process(key);
            std::cout << "Key pressed: " << (char)key << " | Mode: " << (int)keys.getMode() << std::endl;
        }

        processor.process(frame, keys.getMode());
        display.show(frame);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        
        if (keys.getMode() == ProcessMode::FACE) {
            std::cout << "Current Inference Frame Time: " << diff.count() * 1000.0 << " ms (FPS: " << 1.0 / diff.count() << ")\r" << std::flush;
        }
    }

    std::cout << std::endl << "Program finished." << std::endl;
    return 0;
}
