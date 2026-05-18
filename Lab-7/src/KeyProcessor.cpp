#include "KeyProcessor.hpp"

KeyProcessor::KeyProcessor() : currentMode(ProcessMode::NORMAL) {}

void KeyProcessor::process(int key) {
    switch (key) {
        case '0': currentMode = ProcessMode::NORMAL; break;
        case '1': currentMode = ProcessMode::INVERT; break;
        case '2': currentMode = ProcessMode::GRAY;   break;
        case '3': currentMode = ProcessMode::CANNY;  break;
        case '4': currentMode = ProcessMode::BLUR;   break;
        case '5': currentMode = ProcessMode::GLITCH; break;
        case 'f':
        case 'F': currentMode = ProcessMode::FACE;   break;
    }
}

ProcessMode KeyProcessor::getMode() const {
    return currentMode;
}
