#pragma once

#include <atomic>
#include <chrono>
#include <cmath>

/**
 * 极致优化的性能监控器（C++实现）
 */
class PerformanceMonitor {
private:
    // 帧时间统计
    static constexpr int FRAME_TIME_HISTORY_SIZE = 120;
    float m_frameTimes[FRAME_TIME_HISTORY_SIZE];
    int m_currentFrameIndex;
    std::atomic<uint64_t> m_totalFrames;
    
    // FPS计算
    std::chrono::high_resolution_clock::time_point m_lastFPSUpdate;
    std::atomic<uint64_t> m_frameCountSinceLastUpdate;
    std::atomic<float> m_currentFPS;
    
    // 统计信息
    std::atomic<float> m_averageFrameTime;
    std::atomic<float> m_minFrameTime;
    std::atomic<float> m_maxFrameTime;
    std::atomic<float> m_frameTimeVariance;
    
public:
    PerformanceMonitor();
    ~PerformanceMonitor() = default;
    
    // 帧时间更新
    void updateFrameTime(float frameTime);
    
    // 统计数据获取
    float getCurrentFPS() const { return m_currentFPS.load(std::memory_order_relaxed); }
    float getAverageFrameTime() const { return m_averageFrameTime.load(std::memory_order_relaxed); }
    float getMinFrameTime() const { return m_minFrameTime.load(std::memory_order_relaxed); }
    float getMaxFrameTime() const { return m_maxFrameTime.load(std::memory_order_relaxed); }
    float getFrameTimeVariance() const { return m_frameTimeVariance.load(std::memory_order_relaxed); }
    
    // 百分位计算
    float getFrameTimePercentile(float percentile) const;
    
private:
    void updateStatistics();
    void updateFPS();
    float calculateVariance() const;
};