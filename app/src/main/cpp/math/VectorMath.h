#pragma once

#include <cstddef>

// Only include ARM NEON for ARM architectures
#ifdef __ARM_NEON__
    #include <arm_neon.h>
    #define ENABLE_NEON 1
#else
    #define ENABLE_NEON 0
    // Define float32x4_t as a dummy type for non-ARM builds
    typedef float float32x4_t __attribute__((vector_size(16)));
#endif

namespace OptimizedMath {

// 对齐内存分配
#ifndef ALIGNED
#define ALIGNED(x) __attribute__((aligned(x)))
#endif

// 4维向量 - 针对NEON优化
struct ALIGNED(16) Vector4 {
    union {
        struct { float x, y, z, w; };
        float data[4];
#if ENABLE_NEON
        float32x4_t neon;
#endif
    };
    
    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float x, float y, float z, float w = 1.0f) : x(x), y(y), z(z), w(w) {}
    
    // NEON优化的向量运算
    static Vector4 add(const Vector4& a, const Vector4& b);
    static Vector4 subtract(const Vector4& a, const Vector4& b);
    static Vector4 multiply(const Vector4& a, const Vector4& b);
    static Vector4 scale(const Vector4& a, float scalar);
    static float dot(const Vector4& a, const Vector4& b);
    static Vector4 cross(const Vector4& a, const Vector4& b);
    static Vector4 normalize(const Vector4& a);
    static float length(const Vector4& a);
    static float lengthSquared(const Vector4& a);
    
    // 快速近似运算
    static Vector4 fastNormalize(const Vector4& a);
    static float fastLength(const Vector4& a);
    static float fastInverseLength(const Vector4& a);
};

// 3维向量
struct ALIGNED(16) Vector3 {
    union {
        struct { float x, y, z; };
        float data[3];
#if ENABLE_NEON
        float32x4_t neon; // 仍然使用128位对齐以便NEON操作
#endif
    };
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    // 高效向量运算
    static Vector3 add(const Vector3& a, const Vector3& b);
    static Vector3 subtract(const Vector3& a, const Vector3& b);
    static Vector3 multiply(const Vector3& a, const Vector3& b);
    static Vector3 scale(const Vector3& a, float scalar);
    static float dot(const Vector3& a, const Vector3& b);
    static Vector3 cross(const Vector3& a, const Vector3& b);
    static Vector3 normalize(const Vector3& a);
    static float length(const Vector3& a);
    static float lengthSquared(const Vector3& a);
    
    // 快速近似版本
    static Vector3 fastNormalize(const Vector3& a);
    static float fastLength(const Vector3& a);
};

// 2维向量
struct ALIGNED(8) Vector2 {
    float x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
    
    static Vector2 add(const Vector2& a, const Vector2& b);
    static Vector2 subtract(const Vector2& a, const Vector2& b);
    static Vector2 multiply(const Vector2& a, const Vector2& b);
    static Vector2 scale(const Vector2& a, float scalar);
    static float dot(const Vector2& a, const Vector2& b);
    static float length(const Vector2& a);
    static float lengthSquared(const Vector2& a);
    static Vector2 normalize(const Vector2& a);
};

// 批量向量操作 - 针对缓存优化
class BatchVectorOperations {
public:
    // 批量向量加法
    static void addVectors4(const Vector4* a, const Vector4* b, Vector4* result, size_t count);
    static void addVectors3(const Vector3* a, const Vector3* b, Vector3* result, size_t count);
    
    // 批量向量缩放
    static void scaleVectors4(const Vector4* vectors, float scalar, Vector4* result, size_t count);
    static void scaleVectors3(const Vector3* vectors, float scalar, Vector3* result, size_t count);
    
    // 批量点积计算
    static void dotProducts4(const Vector4* a, const Vector4* b, float* results, size_t count);
    static void dotProducts3(const Vector3* a, const Vector3* b, float* results, size_t count);
    
    // 批量归一化
    static void normalizeVectors4(Vector4* vectors, size_t count);
    static void normalizeVectors3(Vector3* vectors, size_t count);
    
    // 快速近似版本
    static void fastNormalizeVectors4(Vector4* vectors, size_t count);
    static void fastNormalizeVectors3(Vector3* vectors, size_t count);
};

} // namespace OptimizedMath
