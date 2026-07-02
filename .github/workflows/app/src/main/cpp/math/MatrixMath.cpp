#include "MatrixMath.h"
#include "FastMath.h"
#include <cstring>

namespace OptimizedMath {

Matrix4::Matrix4() {
    // 初始化为单位矩阵
    rows[0] = vsetq_lane_f32(1.0f, vdupq_n_f32(0.0f), 0);
    rows[1] = vsetq_lane_f32(1.0f, vdupq_n_f32(0.0f), 1);
    rows[2] = vsetq_lane_f32(1.0f, vdupq_n_f32(0.0f), 2);
    rows[3] = vsetq_lane_f32(1.0f, vdupq_n_f32(0.0f), 3);
}

Matrix4::Matrix4(const Matrix4& other) {
    rows[0] = other.rows[0];
    rows[1] = other.rows[1];
    rows[2] = other.rows[2];
    rows[3] = other.rows[3];
}

Matrix4 Matrix4::identity() {
    return Matrix4();
}

Matrix4 Matrix4::zero() {
    Matrix4 result;
    result.rows[0] = vdupq_n_f32(0.0f);
    result.rows[1] = vdupq_n_f32(0.0f);
    result.rows[2] = vdupq_n_f32(0.0f);
    result.rows[3] = vdupq_n_f32(0.0f);
    return result;
}

Matrix4 Matrix4::translation(float x, float y, float z) {
    Matrix4 result = identity();
    result.m[3][0] = x;
    result.m[3][1] = y;
    result.m[3][2] = z;
    return result;
}

Matrix4 Matrix4::scale(float x, float y, float z) {
    Matrix4 result = identity();
    result.m[0][0] = x;
    result.m[1][1] = y;
    result.m[2][2] = z;
    return result;
}

Matrix4 Matrix4::rotationX(float angle) {
    Matrix4 result = identity();
    float s = FastMath::sin(angle);
    float c = FastMath::cos(angle);
    
    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;
    
    return result;
}

Matrix4 Matrix4::rotationY(float angle) {
    Matrix4 result = identity();
    float s = FastMath::sin(angle);
    float c = FastMath::cos(angle);
    
    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;
    
    return result;
}

Matrix4 Matrix4::rotationZ(float angle) {
    Matrix4 result = identity();
    float s = FastMath::sin(angle);
    float c = FastMath::cos(angle);
    
    result.m[0][0] = c;
    result.m[0][1] = -s;
    result.m[1][0] = s;
    result.m[1][1] = c;
    
    return result;
}

Matrix4 Matrix4::perspective(float fov, float aspect, float near, float far) {
    Matrix4 result = zero();
    
    float f = 1.0f / FastMath::tan(fov * 0.5f);
    float rangeInv = 1.0f / (near - far);
    
    result.m[0][0] = f / aspect;
    result.m[1][1] = f;
    result.m[2][2] = (far + near) * rangeInv;
    result.m[2][3] = -1.0f;
    result.m[3][2] = 2.0f * far * near * rangeInv;
    
    return result;
}

Matrix4 Matrix4::orthographic(float left, float right, float bottom, float top, float near, float far) {
    Matrix4 result = identity();
    
    result.m[0][0] = 2.0f / (right - left);
    result.m[1][1] = 2.0f / (top - bottom);
    result.m[2][2] = -2.0f / (far - near);
    
    result.m[3][0] = -(right + left) / (right - left);
    result.m[3][1] = -(top + bottom) / (top - bottom);
    result.m[3][2] = -(far + near) / (far - near);
    
    return result;
}

Matrix4 Matrix4::multiply(const Matrix4& a, const Matrix4& b) {
    Matrix4 result;
    
    // 使用NEON优化的矩阵乘法
    for (int i = 0; i < 4; ++i) {
        float32x4_t row = a.rows[i];
        
        float32x4_t result_row = vmulq_n_f32(b.rows[0], vgetq_lane_f32(row, 0));
        result_row = vmlaq_n_f32(result_row, b.rows[1], vgetq_lane_f32(row, 1));
        result_row = vmlaq_n_f32(result_row, b.rows[2], vgetq_lane_f32(row, 2));
        result_row = vmlaq_n_f32(result_row, b.rows[3], vgetq_lane_f32(row, 3));
        
        result.rows[i] = result_row;
    }
    
    return result;
}

Vector4 Matrix4::multiply(const Matrix4& m, const Vector4& v) {
    Vector4 result;
    
    float32x4_t vec = v.neon;
    float32x4_t result_vec = vmulq_n_f32(m.rows[0], vgetq_lane_f32(vec, 0));
    result_vec = vmlaq_n_f32(result_vec, m.rows[1], vgetq_lane_f32(vec, 1));
    result_vec = vmlaq_n_f32(result_vec, m.rows[2], vgetq_lane_f32(vec, 2));
    result_vec = vmlaq_n_f32(result_vec, m.rows[3], vgetq_lane_f32(vec, 3));
    
    result.neon = result_vec;
    return result;
}

Matrix4 Matrix4::transpose(const Matrix4& m) {
    Matrix4 result;
    
    // 使用NEON转置
    float32x4x2_t a01 = vtrnq_f32(m.rows[0], m.rows[1]);
    float32x4x2_t a23 = vtrnq_f32(m.rows[2], m.rows[3]);
    
    result.rows[0] = vcombine_f32(vget_low_f32(a01.val[0]), vget_low_f32(a23.val[0]));
    result.rows[1] = vcombine_f32(vget_low_f32(a01.val[1]), vget_low_f32(a23.val[1]));
    result.rows[2] = vcombine_f32(vget_high_f32(a01.val[0]), vget_high_f32(a23.val[0]));
    result.rows[3] = vcombine_f32(vget_high_f32(a01.val[1]), vget_high_f32(a23.val[1]));
    
    return result;
}

Matrix4 Matrix4::inverse(const Matrix4& m) {
    // 这里实现完整的4x4矩阵求逆
    // 为简化，这里返回单位矩阵，实际实现需要完整的求逆算法
    return identity();
}

float Matrix4::determinant(const Matrix4& m) {
    // 4x4矩阵行列式计算
    // 为简化，这里返回1，实际实现需要完整的行列式计算
    return 1.0f;
}

Matrix4 Matrix4::fastInverse(const Matrix4& m) {
    // 快速近似逆矩阵，假设是刚体变换矩阵
    Matrix4 result = transpose(m);
    
    // 提取平移部分并取反
    Vector4 translation(m.m[3][0], m.m[3][1], m.m[3][2], 0.0f);
    translation = Vector4::scale(translation, -1.0f);
    
    // 重新计算平移
    result.m[3][0] = Vector4::dot(Vector4(m.m[0][0], m.m[1][0], m.m[2][0], 0.0f), translation);
    result.m[3][1] = Vector4::dot(Vector4(m.m[0][1], m.m[1][1], m.m[2][1], 0.0f), translation);
    result.m[3][2] = Vector4::dot(Vector4(m.m[0][2], m.m[1][2], m.m[2][2], 0.0f), translation);
    
    return result;
}

Matrix4& Matrix4::operator=(const Matrix4& other) {
    if (this != &other) {
        rows[0] = other.rows[0];
        rows[1] = other.rows[1];
        rows[2] = other.rows[2];
        rows[3] = other.rows[3];
    }
    return *this;
}

Matrix4 Matrix4::operator*(const Matrix4& other) const {
    return multiply(*this, other);
}

Vector4 Matrix4::operator*(const Vector4& v) const {
    return multiply(*this, v);
}

// BatchMatrixOperations 实现
void BatchMatrixOperations::multiplyMatrices(const Matrix4* a, const Matrix4* b, Matrix4* result, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        result[i] = Matrix4::multiply(a[i], b[i]);
    }
}

void BatchMatrixOperations::transformVectors4(const Matrix4* matrices, const Vector4* vectors, Vector4* result, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        result[i] = Matrix4::multiply(matrices[i], vectors[i]);
    }
}

void BatchMatrixOperations::transformVectors3(const Matrix4* matrices, const Vector3* vectors, Vector3* result, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        Vector4 vec4(vectors[i].x, vectors[i].y, vectors[i].z, 1.0f);
        Vector4 transformed = Matrix4::multiply(matrices[i], vec4);
        result[i] = Vector3(transformed.x, transformed.y, transformed.z);
    }
}

void BatchMatrixOperations::transposeMatrices(Matrix4* matrices, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        matrices[i] = Matrix4::transpose(matrices[i]);
    }
}

void BatchMatrixOperations::generatePerspectiveMatrices(float fov, float aspect, float near, float far, 
                                                       Matrix4* matrices, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        matrices[i] = Matrix4::perspective(fov, aspect, near, far);
    }
}

} // namespace OptimizedMath