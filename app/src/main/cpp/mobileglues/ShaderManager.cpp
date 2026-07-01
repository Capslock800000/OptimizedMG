#include "ShaderManager.h"
#include "../math/VectorMath.h"
#include "../math/MatrixMath.h"
#include <android/log.h>

#define LOG_TAG "ShaderManager"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

ShaderManager::ShaderManager() : m_currentProgram(nullptr) {}

ShaderManager::~ShaderManager() {
    cleanup();
}

GLuint ShaderManager::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    if (!shader) return 0;

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            std::string log((size_t)len, '\0');
            glGetShaderInfoLog(shader, len, nullptr, log.data());
            LOGE("Shader compile failed: %s", log.c_str());
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool ShaderManager::linkProgram(ShaderProgram& program) {
    program.program = glCreateProgram();
    if (!program.program) return false;

    glAttachShader(program.program, program.vertexShader);
    glAttachShader(program.program, program.fragmentShader);
    glLinkProgram(program.program);

    GLint ok = GL_FALSE;
    glGetProgramiv(program.program, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(program.program, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            std::string log((size_t)len, '\0');
            glGetProgramInfoLog(program.program, len, nullptr, log.data());
            LOGE("Program link failed: %s", log.c_str());
        }
        glDeleteProgram(program.program);
        program.program = 0;
        return false;
    }
    return true;
}

bool ShaderManager::loadShader(const std::string& name, const char* vertexSource, const char* fragmentSource) {
    auto it = m_shaderCache.find(name);
    if (it != m_shaderCache.end()) return true;

    ShaderProgram program{};
    program.vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    if (!program.vertexShader) return false;
    program.fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    if (!program.fragmentShader) {
        glDeleteShader(program.vertexShader);
        return false;
    }
    if (!linkProgram(program)) {
        glDeleteShader(program.vertexShader);
        glDeleteShader(program.fragmentShader);
        return false;
    }

    m_shaderCache.emplace(name, program);
    return true;
}

bool ShaderManager::useShader(const std::string& name) {
    auto it = m_shaderCache.find(name);
    if (it == m_shaderCache.end()) return false;
    m_currentProgram = &it->second;
    glUseProgram(m_currentProgram->program);
    return true;
}

void ShaderManager::unloadShader(const std::string& name) {
    auto it = m_shaderCache.find(name);
    if (it == m_shaderCache.end()) return;
    if (m_currentProgram == &it->second) m_currentProgram = nullptr;
    glDeleteProgram(it->second.program);
    glDeleteShader(it->second.vertexShader);
    glDeleteShader(it->second.fragmentShader);
    m_shaderCache.erase(it);
}

void ShaderManager::cleanup() {
    for (auto& kv : m_shaderCache) {
        glDeleteProgram(kv.second.program);
        glDeleteShader(kv.second.vertexShader);
        glDeleteShader(kv.second.fragmentShader);
    }
    m_shaderCache.clear();
    m_currentProgram = nullptr;
}

GLint ShaderManager::getUniformLocation(const std::string& name) {
    if (!m_currentProgram) return -1;
    auto& cache = m_currentProgram->uniformLocations;
    auto it = cache.find(name);
    if (it != cache.end()) return it->second;
    GLint loc = glGetUniformLocation(m_currentProgram->program, name.c_str());
    cache.emplace(name, loc);
    return loc;
}

GLint ShaderManager::getAttributeLocation(const std::string& name) {
    if (!m_currentProgram) return -1;
    auto& cache = m_currentProgram->attributeLocations;
    auto it = cache.find(name);
    if (it != cache.end()) return it->second;
    GLint loc = glGetAttribLocation(m_currentProgram->program, name.c_str());
    cache.emplace(name, loc);
    return loc;
}

void ShaderManager::setUniform1f(const std::string& name, float value) {
    GLint loc = getUniformLocation(name);
    if (loc >= 0) glUniform1f(loc, value);
}

void ShaderManager::setUniform2f(const std::string& name, float x, float y) {
    GLint loc = getUniformLocation(name);
    if (loc >= 0) glUniform2f(loc, x, y);
}

void ShaderManager::setUniform3f(const std::string& name, float x, float y, float z) {
    GLint loc = getUniformLocation(name);
    if (loc >= 0) glUniform3f(loc, x, y, z);
}

void ShaderManager::setUniform4f(const std::string& name, float x, float y, float z, float w) {
    GLint loc = getUniformLocation(name);
    if (loc >= 0) glUniform4f(loc, x, y, z, w);
}

void ShaderManager::setUniformMatrix4f(const std::string& name, const OptimizedMath::Matrix4& matrix) {
    GLint loc = getUniformLocation(name);
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.data);
}

void ShaderManager::setUniformVector4(const std::string& name, const OptimizedMath::Vector4& vector) {
    GLint loc = getUniformLocation(name);
    if (loc >= 0) glUniform4f(loc, vector.x, vector.y, vector.z, vector.w);
}

void ShaderManager::setUniform1fArray(const std::string& name, const float* values, int count) {
    GLint loc = getUniformLocation(name);
    if (loc >= 0) glUniform1fv(loc, count, values);
}

void ShaderManager::setUniformMatrix4fArray(const std::string& name, const OptimizedMath::Matrix4* matrices, int count) {
    GLint loc = getUniformLocation(name);
    if (loc >= 0) glUniformMatrix4fv(loc, count, GL_FALSE, &matrices[0].data[0]);
}
