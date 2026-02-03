#pragma once

#include <atomic>
#include <chrono>
#include "math/VectorMath.h"
#include "math/MatrixMath.h"

class PerformanceMonitor;

/**
 * 极致优化的渲染器核心类
 */
class OptimizedRenderer {
private:
    // 渲染状态
    bool m_initialized;
    bool m_surfaceCreated;
    int m_width, m_height;
    
    // 性能监控
    PerformanceMonitor* m_performanceMonitor;
    std::atomic<uint64_t> m_frameCount;
    std::chrono::high_resolution_clock::time_point m_lastFPSUpdate;
    float m_currentFPS;
    float m_averageFrameTime;
    
    // 渲染数据
    OptimizedMath::Matrix4 m_projectionMatrix;
    OptimizedMath::Matrix4 m_viewMatrix;
    OptimizedMath::Matrix4 m_modelMatrix;
    
    // 着色器程序
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_shaderProgram;
    
    // 缓冲区对象
    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    GLuint m_vertexArray;
    
public:
    OptimizedRenderer();
    ~OptimizedRenderer();
    
    bool initialize();
    void cleanup();
    void onSurfaceCreated();
    void onSurfaceChanged(int width, int height);
    void onDrawFrame();
    
    // 性能监控
    void setPerformanceMonitor(PerformanceMonitor* monitor);
    float getCurrentFPS() const { return m_currentFPS; }
    float getAverageFrameTime() const { return m_averageFrameTime; }
    
private:
    // 初始化方法
    bool initializeShaders();
    bool initializeBuffers();
    void initializeMatrices();
    
    // 渲染方法
    void renderFrame();
    void updateFPS();
    
    // 工具方法
    GLuint compileShader(const char* source, GLenum type);
    void checkGLError(const char* operation);
};