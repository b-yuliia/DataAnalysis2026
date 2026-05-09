#ifndef KEY_PROCESSOR_HPP
#define KEY_PROCESSOR_HPP

enum class ProcessMode {
    NORMAL, INVERT, GRAY, CANNY, BLUR, GLITCH
};

class KeyProcessor {
public:
    KeyProcessor();
    void process(int key);
    ProcessMode getMode() const;

private:
    ProcessMode currentMode;
};

#endif
