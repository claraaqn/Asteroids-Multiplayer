#ifndef PERFORMANCETRACKER_H
#define PERFORMANCETRACKER_H

#include <chrono>
#include <iomanip>
#include <iostream>

class PerformanceTracker {
private:
    std::chrono::high_resolution_clock::time_point lastPrint;
    std::chrono::high_resolution_clock::time_point frameStart;
    int frameCount = 0;
    float minFrameTime = 9999.0f;
    float maxFrameTime = 0.0f;
    float totalFrameTime = 0.0f;
    
public:
    PerformanceTracker();
    void startFrame();
    void endFrame();
    void printStats();
    void resetStats();
};

#endif