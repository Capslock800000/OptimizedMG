#include <jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <cstring>
#include <atomic>
#include "math/VectorMath.h"
#include "math/MatrixMath.h"
#include "math/FastMath.h"
#include "mobileglues/OptimizedRenderer.h"

#define LOG_TAG "OptimizedMG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// 全局渲染器实例
static OptimizedRenderer* g_renderer = nullptr;
static std::atomic<bool> g_initialized{false};

// JNI函数声明
extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_optimizedmg_renderer_OptimizedRenderer_nativeInit(JNIEnv* env, jobject thiz) {
    if (g_initialized) {
        return JNI_TRUE;
    }
    
    try {
        g_renderer = new OptimizedRenderer();
        if (g_renderer->initialize()) {
            g_initialized = true;
            LOGI("OptimizedRenderer initialized successfully");
            return JNI_TRUE;
        } else {
            delete g_renderer;
            g_renderer = nullptr;
            LOGE("Failed to initialize OptimizedRenderer");
            return JNI_FALSE;
        }
    } catch (const std::exception& e) {
        LOGE("Exception during initialization: %s", e.what());
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL
Java_com_optimizedmg_renderer_OptimizedRenderer_nativeCleanup(JNIEnv* env, jobject thiz) {
    if (g_renderer) {
        g_renderer->cleanup();
        delete g_renderer;
        g_renderer = nullptr;
    }
    g_initialized = false;
    LOGI("OptimizedRenderer cleaned up");
}

JNIEXPORT void JNICALL
Java_com_optimizedmg_renderer_OptimizedRenderer_nativeOnSurfaceCreated(JNIEnv* env, jobject thiz) {
    if (g_renderer) {
        g_renderer->onSurfaceCreated();
    }
}

JNIEXPORT void JNICALL
Java_com_optimizedmg_renderer_OptimizedRenderer_nativeOnSurfaceChanged(JNIEnv* env, jobject thiz, 
                                                                      jint width, jint height) {
    if (g_renderer) {
        g_renderer->onSurfaceChanged(width, height);
    }
}

JNIEXPORT void JNICALL
Java_com_optimizedmg_renderer_OptimizedRenderer_nativeOnDrawFrame(JNIEnv* env, jobject thiz) {
    if (g_renderer) {
        g_renderer->onDrawFrame();
    }
}

JNIEXPORT void JNICALL
Java_com_optimizedmg_renderer_OptimizedRenderer_nativeSetPerformanceMonitor(JNIEnv* env, jobject thiz, 
                                                                           jlong ptr) {
    if (g_renderer) {
        g_renderer->setPerformanceMonitor(reinterpret_cast<PerformanceMonitor*>(ptr));
    }
}

JNIEXPORT jfloat JNICALL
Java_com_optimizedmg_renderer_OptimizedRenderer_nativeGetFPS(JNIEnv* env, jobject thiz) {
    if (g_renderer) {
        return g_renderer->getCurrentFPS();
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_optimizedmg_renderer_OptimizedRenderer_nativeGetFrameTime(JNIEnv* env, jobject thiz) {
    if (g_renderer) {
        return g_renderer->getAverageFrameTime();
    }
    return 0.0f;
}

// PerformanceMonitor JNI函数
JNIEXPORT jlong JNICALL
Java_com_optimizedmg_core_PerformanceMonitor_nativeCreate(JNIEnv* env, jobject thiz) {
    auto monitor = new PerformanceMonitor();
    return reinterpret_cast<jlong>(monitor);
}

JNIEXPORT void JNICALL
Java_com_optimizedmg_core_PerformanceMonitor_nativeDestroy(JNIEnv* env, jobject thiz, jlong ptr) {
    auto monitor = reinterpret_cast<PerformanceMonitor*>(ptr);
    if (monitor) {
        delete monitor;
    }
}

JNIEXPORT void JNICALL
Java_com_optimizedmg_core_PerformanceMonitor_nativeUpdateFrameTime(JNIEnv* env, jobject thiz, 
                                                                  jlong ptr, jfloat frameTime) {
    auto monitor = reinterpret_cast<PerformanceMonitor*>(ptr);
    if (monitor) {
        monitor->updateFrameTime(frameTime);
    }
}

JNIEXPORT jfloat JNICALL
Java_com_optimizedmg_core_PerformanceMonitor_nativeGetFPS(JNIEnv* env, jobject thiz, jlong ptr) {
    auto monitor = reinterpret_cast<PerformanceMonitor*>(ptr);
    if (monitor) {
        return monitor->getCurrentFPS();
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_optimizedmg_core_PerformanceMonitor_nativeGetAverageFrameTime(JNIEnv* env, jobject thiz, jlong ptr) {
    auto monitor = reinterpret_cast<PerformanceMonitor*>(ptr);
    if (monitor) {
        return monitor->getAverageFrameTime();
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_optimizedmg_core_PerformanceMonitor_nativeGetFrameTimeVariance(JNIEnv* env, jobject thiz, jlong ptr) {
    auto monitor = reinterpret_cast<PerformanceMonitor*>(ptr);
    if (monitor) {
        return monitor->getFrameTimeVariance();
    }
    return 0.0f;
}

} // extern "C"