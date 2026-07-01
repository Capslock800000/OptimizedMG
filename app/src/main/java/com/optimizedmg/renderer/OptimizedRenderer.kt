package com.capslock800000.optimizedmg.renderer

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.Log
import com.optimizedmg.core.PerformanceMonitor
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

/**
 * 极致优化的渲染器，专注于数学逻辑和运行速度
 */
class OptimizedRenderer(private val context: Context) : GLSurfaceView.Renderer {
    
    private external fun nativeInit(): Boolean
    private external fun nativeCleanup()
    private external fun nativeOnSurfaceCreated()
    private external fun nativeOnSurfaceChanged(width: Int, height: Int)
    private external fun nativeOnDrawFrame()
    private external fun nativeSetPerformanceMonitor(ptr: Long)
    private external fun nativeGetFPS(): Float
    private external fun nativeGetFrameTime(): Float
    
    private var isInitialized = false
    private var performanceMonitor: PerformanceMonitor? = null
    private var nativePerformanceMonitorPtr: Long = 0
    
    fun setPerformanceMonitor(monitor: PerformanceMonitor) {
        this.performanceMonitor = monitor
        this.nativePerformanceMonitorPtr = monitor.getNativePtr()
        if (isInitialized) {
            nativeSetPerformanceMonitor(nativePerformanceMonitorPtr)
        }
    }
    
    fun resume() {
        // 渲染器恢复逻辑
    }
    
    fun pause() {
        // 渲染器暂停逻辑
    }
    
    fun cleanup() {
        if (isInitialized) {
            nativeCleanup()
            isInitialized = false
        }
    }
    
    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        if (!isInitialized) {
            isInitialized = nativeInit()
            if (isInitialized && nativePerformanceMonitorPtr != 0L) {
                nativeSetPerformanceMonitor(nativePerformanceMonitorPtr)
            }
        }
        nativeOnSurfaceCreated()
    }
    
    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        nativeOnSurfaceChanged(width, height)
    }
    
    override fun onDrawFrame(gl: GL10?) {
        val startTime = System.nanoTime()
        nativeOnDrawFrame()
        val endTime = System.nanoTime()
        
        val frameTime = (endTime - startTime) / 1_000_000.0f // 转换为毫秒
        performanceMonitor?.updateFrameTime(frameTime)
    }
    
    fun getCurrentFPS(): Float {
        return if (isInitialized) nativeGetFPS() else 0f
    }
    
    fun getAverageFrameTime(): Float {
        return if (isInitialized) nativeGetFrameTime() else 0f
    }
    
    companion object {
        init {
            System.loadLibrary("optimizedmg")
        }
    }
}