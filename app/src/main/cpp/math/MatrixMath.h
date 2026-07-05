#pragma once

#include "VectorMath.h"
#include <arm_neon.h>

namespace OptimizedMath {

// 4x4 矩阵 - 针对NEON和缓存优化
struct ALIGNED(16) Matrix4 {
    union {
        float m[4][4];
        float data[16];
        float32x4_t rows[4]; // NEON友好的行存储
    };
    
    Matrix4();
    Matrix4(const Matrix4& other);
    
    // 静态构造方法
    static Matrix4 identity();
    static Matrix4 zero();
    static Matrix4 translation(float x, float y, float z);
    static Matrix4 scale(float x, float y, float z);
    static Matrix4 rotationX(float angle);
    static Matrix4 rotationY(float angle);
    static Matrix4 rotationZ(float angle);
    static Matrix4 perspective(float fov, float aspect, float near, float far);
    static Matrix4 orthographic(float left, float right, float bottom, float top, float near, float far);
    
    // 矩阵运算
    static Matrix4 multiply(const Matrix4& a, const Matrix4& b);
    static Vector4 multiply(const Matrix4& m, const Vector4& v);
    static Matrix4 transpose(const Matrix4& m);
    static Matrix4 inverse(const Matrix4& m);
    static float determinant(const Matrix4& m);
    
    // 快速近似版本
    static Matrix4 fastInverse(const Matrix4& m);
    
    // 运算符重载
    Matrix4& operator=(const Matrix4& other);
    Matrix4 operator*(const Matrix4& other) const;
    Vector4 operator*(const Vector4& v) const;
};

// 批量矩阵操作
class BatchMatrixOperations {
public:
    // 批量矩阵乘法
    static void multiplyMatrices(const Matrix4* a, const Matrix4* b, Matrix4* result, size_t count);
    
    // 批量矩阵-向量乘法
    static void transformVectors4(const Matrix4* matrices, const Vector4* vectors, Vector4* result, size_t count);
    static void transformVectors3(const Matrix4* matrices, const Vector3* vectors, Vector3* result, size_t count);
    
    // 批量矩阵转置
    static void transposeMatrices(Matrix4* matrices, size_t count);
    
    // 批量透视矩阵生成
    static void generatePerspectiveMatrices(float fov, float aspect, float near, float far, 
                                          Matrix4* matrices, size_t count);
};

} // namespace OptimizedMath