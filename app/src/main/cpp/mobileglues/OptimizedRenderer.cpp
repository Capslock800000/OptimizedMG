#include "OptimizedRenderer.h"
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <cstdint>
#include <chrono>
#include "math/FastMath.h"

#define LOG_TAG "OptimizedRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// 简单的顶点着色器
static const char* VERTEX_SHADER_SOURCE = R"(
    attribute vec4 aPosition;
    attribute vec4 aColor;
    uniform mat4 uMVPMatrix;
    varying vec4 vColor;
    
    void main() {
        gl_Position = uMVPMatrix * aPosition;
        vColor = aColor;
    }
)";

// 简单的片段着色器
static const char* FRAGMENT_SHADER_SOURCE = R"(
    precision mediump float;
    varying vec4 vColor;
    
    void main() {
        gl_FragColor = vColor;
    }
)";

// 测试顶点数据 - 一个彩色的三角形
static const float VERTEX_DATA[] = {
    // 位置 (x, y, z, w)      颜色 (r, g, b, a)
     0.0f,  0.5f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f
};

static const uint16_t INDEX_DATA[] = { 0, 1, 2 };

OptimizedRenderer::OptimizedRenderer()
    : m_initialized(false)
    , m_surfaceCreated(false)
    , m_width(0)
    , m_height(0)
    , m_performanceMonitor(nullptr)
    , m_frameCount(0)
    , m_currentFPS(0.0f)
    , m_averageFrameTime(0.0f)
    , m_lastFPSUpdate(std::chrono::high_resolution_clock::now())
    , m_vertexShader(0)
    , m_fragmentShader(0)
    , m_shaderProgram(0)
    , m_vertexBuffer(0)
    , m_indexBuffer(0)
    , m_vertexArray(0) {
}

OptimizedRenderer::~OptimizedRenderer() {
    cleanup();
}

bool OptimizedRenderer::initialize() {
    if (m_initialized) {
        return true;
    }
    
    LOGI("Initializing OptimizedRenderer");
    
    // 初始化OpenGL ES
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    LOGI("OpenGL ES Version: %s", version ? version : "Unknown");
    LOGI("Renderer: %s", renderer ? renderer : "Unknown");
    
    // 检查必要的扩展
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (extensions) {
        LOGI("Supported extensions: %s", extensions);
    }
    
    // 初始化着色器和缓冲区
    if (!initializeShaders()) {
        LOGE("Failed to initialize shaders");
        return false;
    }
    
    if (!initializeBuffers()) {
        LOGE("Failed to initialize buffers");
        return false;
    }
    
    initializeMatrices();
    
    m_initialized = true;
    LOGI("OptimizedRenderer initialized successfully");
    return true;
}

void OptimizedRenderer::cleanup() {
    if (m_vertexShader) {
        glDeleteShader(m_vertexShader);
        m_vertexShader = 0;
    }
    
    if (m_fragmentShader) {
        glDeleteShader(m_fragmentShader);
        m_fragmentShader = 0;
    }
    
    if (m_shaderProgram) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }
    
    if (m_vertexBuffer) {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
    }
    
    if (m_indexBuffer) {
        glDeleteBuffers(1, &m_indexBuffer);
        m_indexBuffer = 0;
    }
    
    if (m_vertexArray) {
        glDeleteVertexArrays(1, &m_vertexArray);
        m_vertexArray = 0;
    }
    
    m_initialized = false;
    m_surfaceCreated = false;
    LOGI("OptimizedRenderer cleaned up");
}

void OptimizedRenderer::onSurfaceCreated() {
    LOGI("Surface created");
    
    // 设置清屏颜色
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 启用背面剔除
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    m_surfaceCreated = true;
}

void OptimizedRenderer::onSurfaceChanged(int width, int height) {
    LOGI("Surface changed: %dx%d", width, height);
    
    m_width = width;
    m_height = height;
    
    // 设置视口
    glViewport(0, 0, width, height);
    
    // 更新投影矩阵
    float aspect = (float)width / (float)height;
    m_projectionMatrix = OptimizedMath::Matrix4::perspective(
        OptimizedMath::FastMath::PI / 3.0f, // 60度FOV
        aspect,
        0.1f,  // near
        100.0f // far
    );
}

void OptimizedRenderer::onDrawFrame() {
    if (!m_initialized || !m_surfaceCreated) {
        return;
    }
    
    auto frameStart = std::chrono::high_resolution_clock::now();
    
    // 清屏
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 渲染帧
    renderFrame();
    
    // 更新帧计数
    m_frameCount++;
    
    // 更新FPS计算
    updateFPS();
    
    // 计算帧时间（用于性能监控）
    auto frameEnd = std::chrono::high_resolution_clock::now();
    auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);
    m_averageFrameTime = frameDuration.count() / 1000.0f; // 转换为毫秒
    
    // 更新性能监控器
    if (m_performanceMonitor) {
        // 这里应该调用性能监控器的更新方法
        // m_performanceMonitor->updateFrameTime(m_averageFrameTime);
    }
}

void OptimizedRenderer::setPerformanceMonitor(PerformanceMonitor* monitor) {
    m_performanceMonitor = monitor;
}

bool OptimizedRenderer::initializeShaders() {
    // 编译顶点着色器
    m_vertexShader = compileShader(VERTEX_SHADER_SOURCE, GL_VERTEX_SHADER);
    if (!m_vertexShader) {
        return false;
    }
    
    // 编译片段着色器
    m_fragmentShader = compileShader(FRAGMENT_SHADER_SOURCE, GL_FRAGMENT_SHADER);
    if (!m_fragmentShader) {
        return false;
    }
    
    // 创建着色器程序
    m_shaderProgram = glCreateProgram();
    if (!m_shaderProgram) {
        LOGE("Failed to create shader program");
        return false;
    }
    
    // 附加着色器
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    
    // 链接程序
    glLinkProgram(m_shaderProgram);
    
    // 检查链接状态
    GLint linkStatus;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_shaderProgram, sizeof(infoLog), nullptr, infoLog);
        LOGE("Shader program linking failed: %s", infoLog);
        return false;
    }
    
    LOGI("Shaders initialized successfully");
    return true;
}

bool OptimizedRenderer::initializeBuffers() {
    // 生成顶点数组对象
    glGenVertexArrays(1, &m_vertexArray);
    glBindVertexArray(m_vertexArray);
    
    // 生成顶点缓冲区
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_DATA), VERTEX_DATA, GL_STATIC_DRAW);
    
    // 生成索引缓冲区
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDEX_DATA), INDEX_DATA, GL_STATIC_DRAW);
    
    // 设置顶点属性
    glEnableVertexAttribArray(0); // 位置
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1); // 颜色
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
    
    // 解绑
    glBindVertexArray(0);
    
    LOGI("Buffers initialized successfully");
    return true;
}

void OptimizedRenderer::initializeMatrices() {
    // 初始化模型矩阵为单位矩阵
    m_modelMatrix = OptimizedMath::Matrix4::identity();
    
    // 初始化视图矩阵 - 稍微向后移动相机
    m_viewMatrix = OptimizedMath::Matrix4::translation(0.0f, 0.0f, -2.0f);
}

void OptimizedRenderer::renderFrame() {
    // 使用着色器程序
    glUseProgram(m_shaderProgram);
    
    // 绑定顶点数组
    glBindVertexArray(m_vertexArray);
    
    // 计算MVP矩阵
    OptimizedMath::Matrix4 mvp = OptimizedMath::Matrix4::multiply(
        m_projectionMatrix,
        OptimizedMath::Matrix4::multiply(m_viewMatrix, m_modelMatrix)
    );
    
    // 设置MVP矩阵uniform
    GLint mvpLocation = glGetUniformLocation(m_shaderProgram, "uMVPMatrix");
    if (mvpLocation != -1) {
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp.data);
    }
    
    // 轻微旋转模型矩阵以产生动画效果
    static float rotation = 0.0f;
    rotation += 0.01f;
    m_modelMatrix = OptimizedMath::Matrix4::rotationY(rotation);
    
    // 绘制三角形
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);
    
    // 解绑
    glBindVertexArray(0);
}

void OptimizedRenderer::updateFPS() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - m_lastFPSUpdate
    ).count();
    
    // 每秒更新一次FPS
    if (elapsed >= 1000) {
        m_currentFPS = (m_frameCount * 1000.0f) / elapsed;
        m_frameCount = 0;
        m_lastFPSUpdate = currentTime;
        
        LOGI("FPS: %.1f, Frame Time: %.2f ms", m_currentFPS, m_averageFrameTime);
    }
}

GLuint OptimizedRenderer::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    if (!shader) {
        LOGE("Failed to create shader");
        return 0;
    }
    
    // 编译着色器
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    // 检查编译状态
    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        LOGE("Shader compilation failed: %s", infoLog);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

void OptimizedRenderer::checkGLError(const char* operation) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        LOGE("OpenGL error during %s: 0x%04X", operation, error);
    }
}