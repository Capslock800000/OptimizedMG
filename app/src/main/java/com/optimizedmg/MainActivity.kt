package com.capslock800000.optimizedmg

import android.opengl.GLSurfaceView
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.ui.Modifier
import com.capslock800000.optimizedmg.core.PerformanceMonitor
import com.capslock800000.optimizedmg.renderer.OptimizedRenderer
import com.capslock800000.optimizedmg.ui.Theme
import com.capslock800000.optimizedmg.ui.MainScreen

class MainActivity : AppCompatActivity() {
    companion object {
        init {
            try {
                System.loadLibrary("optimizedmg")
            } catch (e: UnsatisfiedLinkError) {
                android.util.Log.e("OptimizedMG", "Failed to load native library", e)
            }
        }
    }
    private lateinit var renderer: OptimizedRenderer
    private lateinit var glSurfaceView: GLSurfaceView
    private lateinit var performanceMonitor: PerformanceMonitor
    
    override fun onCreate(savedInstanceState: Bundle?) {
        Thread.setDefaultUncaughtExceptionHandler { _, e ->
            android.util.Log.e("OptimizedMG", "Uncaught exception", e)
        }
        super.onCreate(savedInstanceState)
        
        // 初始化性能监控
        performanceMonitor = PerformanceMonitor()
        
        // 初始化渲染器
        renderer = OptimizedRenderer(this).apply {
            setPerformanceMonitor(performanceMonitor)
        }
        
        // 设置Compose UI
        setContent {
            Theme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    MainScreen(
                        renderer = renderer,
                        performanceMonitor = performanceMonitor
                    )
                }
            }
        }
    }
    
    override fun onResume() {
        super.onResume()
        renderer.resume()
    }
    
    override fun onPause() {
        super.onPause()
        renderer.pause()
    }
    
    override fun onDestroy() {
        super.onDestroy()
        renderer.cleanup()
    }
}