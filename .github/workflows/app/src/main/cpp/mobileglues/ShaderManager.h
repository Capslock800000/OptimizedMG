#pragma once

#include <GLES2/gl2.h>
#include <string>
#include <unordered_map>

namespace OptimizedMath {
    class Vector4;
    class Matrix4;
}

/**
 * 极致优化的着色器管理器，支持缓存和批量操作
 */
class ShaderManager {
private:
    struct ShaderProgram {
        GLuint program;
        GLuint vertexShader;
        GLuint fragmentShader;
        std::unordered_map<std::string, GLint> uniformLocations;
        std::unordered_map<std::string, GLint> attributeLocations;
    };
    
    std::unordered_map<std::string, ShaderProgram> m_shaderCache;
    ShaderProgram* m_currentProgram;
    
public:
    ShaderManager();
    ~ShaderManager();
    
    // 着色器管理
    bool loadShader(const std::string& name, 
                   const char* vertexSource, 
                   const char* fragmentSource);
    bool useShader(const std::string& name);
    void unloadShader(const std::string& name);
    void cleanup();
    
    // Uniform设置 - 使用快速路径
    void setUniform1f(const std::string& name, float value);
    void setUniform2f(const std::string& name, float x, float y);
    void setUniform3f(const std::string& name, float x, float y, float z);
    void setUniform4f(const std::string& name, float x, float y, float z, float w);
    void setUniformMatrix4f(const std::string& name, const OptimizedMath::Matrix4& matrix);
    void setUniformVector4(const std::string& name, const OptimizedMath::Vector4& vector);
    
    // 批量Uniform设置
    void setUniform1fArray(const std::string& name, const float* values, int count);
    void setUniformMatrix4fArray(const std::string& name, const OptimizedMath::Matrix4* matrices, int count);
    
private:
    GLint getUniformLocation(const std::string& name);
    GLint getAttributeLocation(const std::string& name);
    GLuint compileShader(const char* source, GLenum type);
    bool linkProgram(ShaderProgram& program);
};