#include "PerformanceTracker.h"
#include <iostream>
#include <iomanip>

PerformanceTracker::PerformanceTracker() : 
    lastPrint(std::chrono::high_resolution_clock::now()) {}

void PerformanceTracker::startFrame() {
    frameStart = std::chrono::high_resolution_clock::now();
}

void PerformanceTracker::endFrame() {
    auto frameEnd = std::chrono::high_resolution_clock::now();
    float frameTime = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart).count() / 1000.0f;
    
    // Atualiza estatísticas
    frameCount++;
    totalFrameTime += frameTime;
    if (frameTime < minFrameTime) minFrameTime = frameTime;
    if (frameTime > maxFrameTime) maxFrameTime = frameTime;
    
    // Verifica se é hora de imprimir (a cada 1 segundo)
    auto currentTime = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastPrint).count() >= 1) {
        printStats();
        resetStats();
        lastPrint = currentTime;
    }
}

void PerformanceTracker::printStats() {
    if (frameCount == 0) return;
    
    float avgFrameTime = totalFrameTime / frameCount;
    float fps = 1000.0f / avgFrameTime;
    
    std::cout << "\n--- Performance Stats ---\n";
    std::cout << "FPS: " << std::fixed << std::setprecision(1) << fps << "\n";
    std::cout << "Frame Time (ms): Avg=" << std::setprecision(3) << avgFrameTime 
              << " Min=" << minFrameTime 
              << " Max=" << maxFrameTime << "\n";
    std::cout << "Frames analisados: " << frameCount << "\n";
    std::cout << "------------------------\n";
}

void PerformanceTracker::resetStats() {
    frameCount = 0;
    totalFrameTime = 0.0f;
    minFrameTime = 9999.0f;
    maxFrameTime = 0.0f;
}