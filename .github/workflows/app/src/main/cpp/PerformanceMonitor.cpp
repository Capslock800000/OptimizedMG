#include "PerformanceMonitor.h"
#include <algorithm>
#include <cmath>

PerformanceMonitor::PerformanceMonitor()
    : m_currentFrameIndex(0)
    , m_totalFrames(0)
    , m_frameCountSinceLastUpdate(0)
    , m_currentFPS(0.0f)
    , m_averageFrameTime(0.0f)
    , m_minFrameTime(1000.0f) // 初始化为1秒
    , m_maxFrameTime(0.0f)
    , m_frameTimeVariance(0.0f)
    , m_lastFPSUpdate(std::chrono::high_resolution_clock::now()) {
    
    // 初始化帧时间数组
    for (int i = 0; i < FRAME_TIME_HISTORY_SIZE; ++i) {
        m_frameTimes[i] = 0.0f;
    }
}

void PerformanceMonitor::updateFrameTime(float frameTime) {
    // 更新帧时间历史
    m_frameTimes[m_currentFrameIndex] = frameTime;
    m_currentFrameIndex = (m_currentFrameIndex + 1) % FRAME_TIME_HISTORY_SIZE;
    
    // 更新总帧数
    m_totalFrames++;
    m_frameCountSinceLastUpdate++;
    
    // 更新统计数据
    updateStatistics();
    
    // 更新FPS
    updateFPS();
}

void PerformanceMonitor::updateStatistics() {
    int validFrames = std::min(static_cast<int>(m_totalFrames), FRAME_TIME_HISTORY_SIZE);
    if (validFrames == 0) return;
    
    float sum = 0.0f;
    float minTime = m_frameTimes[0];
    float maxTime = m_frameTimes[0];
    
    // 计算总和、最小值和最大值
    for (int i = 0; i < validFrames; ++i) {
        float time = m_frameTimes[i];
        sum += time;
        
        if (time < minTime) minTime = time;
        if (time > maxTime) maxTime = time;
    }
    
    // 更新原子变量
    m_averageFrameTime.store(sum / validFrames, std::memory_order_relaxed);
    m_minFrameTime.store(minTime, std::memory_order_relaxed);
    m_maxFrameTime.store(maxTime, std::memory_order_relaxed);
    
    // 计算方差
    m_frameTimeVariance.store(calculateVariance(), std::memory_order_relaxed);
}

void PerformanceMonitor::updateFPS() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - m_lastFPSUpdate
    ).count();
    
    // 每秒更新一次FPS
    if (elapsed >= 1000) {
        float fps = (m_frameCountSinceLastUpdate * 1000.0f) / elapsed;
        m_currentFPS.store(fps, std::memory_order_relaxed);
        
        m_frameCountSinceLastUpdate = 0;
        m_lastFPSUpdate = currentTime;
    }
}

float PerformanceMonitor::calculateVariance() const {
    int validFrames = std::min(static_cast<int>(m_totalFrames), FRAME_TIME_HISTORY_SIZE);
    if (validFrames < 2) return 0.0f;
    
    float mean = m_averageFrameTime.load(std::memory_order_relaxed);
    float sumSq = 0.0f;
    
    for (int i = 0; i < validFrames; ++i) {
        float diff = m_frameTimes[i] - mean;
        sumSq += diff * diff;
    }
    
    return sumSq / (validFrames - 1);
}

float PerformanceMonitor::getFrameTimePercentile(float percentile) const {
    int validFrames = std::min(static_cast<int>(m_totalFrames), FRAME_TIME_HISTORY_SIZE);
    if (validFrames == 0) return 0.0f;
    
    // 创建临时数组进行排序
    float sortedTimes[FRAME_TIME_HISTORY_SIZE];
    std::copy(m_frameTimes, m_frameTimes + validFrames, sortedTimes);
    std::sort(sortedTimes, sortedTimes + validFrames);
    
    // 计算百分位索引
    int index = static_cast<int>(percentile * validFrames / 100.0f);
    index = std::max(0, std::min(index, validFrames - 1));
    
    return sortedTimes[index];
}