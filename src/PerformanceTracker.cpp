#include <chrono>
#include <iomanip> // Para std::fixed e std::setprecision

// Classe para medição de performance
class PerformanceTracker {
private:
    std::chrono::high_resolution_clock::time_point lastPrint;
    std::chrono::high_resolution_clock::time_point frameStart;
    int frameCount = 0;
    float minFrameTime = 9999.0f;
    float maxFrameTime = 0.0f;
    float totalFrameTime = 0.0f;
    
public:
    PerformanceTracker() : lastPrint(std::chrono::high_resolution_clock::now()) {}
    
    void startFrame() {
        frameStart = std::chrono::high_resolution_clock::now();
    }
    
    void endFrame() {
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
    
    void printStats() {
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
    
    void resetStats() {
        frameCount = 0;
        totalFrameTime = 0.0f;
        minFrameTime = 9999.0f;
        maxFrameTime = 0.0f;
    }
}