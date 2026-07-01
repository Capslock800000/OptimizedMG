package com.optimizedmg.core

import java.util.concurrent.atomic.AtomicLong
import kotlin.math.sqrt

/**
 * 极致优化的性能监控器，使用原子操作和高效数学计算
 */
class PerformanceMonitor {
    
    private external fun nativeCreate(): Long
    private external fun nativeDestroy(ptr: Long)
    private external fun nativeUpdateFrameTime(ptr: Long, frameTime: Float)
    private external fun nativeGetFPS(ptr: Long): Float
    private external fun nativeGetAverageFrameTime(ptr: Long): Float
    private external fun nativeGetFrameTimeVariance(ptr: Long): Float
    
    private val nativePtrHandle: Long by lazy(LazyThreadSafetyMode.NONE) { nativeCreate() }
    private val frameTimes = FloatArray(120) // 2秒数据（假设60FPS）
    private var currentIndex = 0
    private var totalFrames = 0L
    
    // 使用原子操作确保线程安全
    private val lastUpdateTime = AtomicLong(System.nanoTime())
    private val frameCount = AtomicLong(0)
    
    fun getNativePtr(): Long = nativePtrHandle
    
    fun updateFrameTime(frameTime: Float) {
        frameTimes[currentIndex] = frameTime
        currentIndex = (currentIndex + 1) % frameTimes.size
        totalFrames++
        
        frameCount.incrementAndGet()
        nativeUpdateFrameTime(nativePtrHandle, frameTime)
    }
    
    fun getCurrentFPS(): Float {
        return nativeGetFPS(nativePtrHandle)
    }
    
    fun getAverageFrameTime(): Float {
        return nativeGetAverageFrameTime(nativePtrHandle)
    }
    
    fun getFrameTimeVariance(): Float {
        return nativeGetFrameTimeVariance(nativePtrHandle)
    }
    
    fun getFrameTimePercentile(percentile: Float): Float {
        if (totalFrames == 0L) return 0f
        
        val sortedTimes = frameTimes.copyOf().sortedArray()
        val index = (sortedTimes.size * percentile / 100).toInt().coerceIn(0, sortedTimes.size - 1)
        return sortedTimes[index]
    }
    
    /**
     * 使用快速平方根近似的标准差计算
     */
    fun getFrameTimeStdDev(): Float {
        if (totalFrames < 2) return 0f
        
        val mean = getAverageFrameTime()
        var sumSq = 0f
        
        for (i in 0 until minOf(frameTimes.size, totalFrames.toInt())) {
            val diff = frameTimes[i] - mean
            sumSq += diff * diff
        }
        
        val variance = sumSq / minOf(frameTimes.size, totalFrames.toInt())
        return fastSqrt(variance)
    }
    
    /**
     * 快速平方根近似 - 使用位操作和牛顿迭代法
     */
    private fun fastSqrt(x: Float): Float {
        if (x <= 0f) return 0f
        
        // 初始估计使用位操作
        var i = java.lang.Float.floatToIntBits(x)
        i = 0x5f3759df - (i shr 1)
        var y = java.lang.Float.intBitsToFloat(i)
        
        // 一次牛顿迭代提高精度
        y = y * (1.5f - 0.5f * x * y * y)
        return 1.0f / y * x // 修正结果
    }
    
    protected fun finalize() {
        nativeDestroy(nativePtrHandle)
    }
}