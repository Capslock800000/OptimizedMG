#include "VectorMath.h"
#include "FastMath.h"
#include <cstring>

namespace OptimizedMath {

// Vector4 实现
Vector4 Vector4::add(const Vector4& a, const Vector4& b) {
    Vector4 result;
    result.neon = vaddq_f32(a.neon, b.neon);
    return result;
}

Vector4 Vector4::subtract(const Vector4& a, const Vector4& b) {
    Vector4 result;
    result.neon = vsubq_f32(a.neon, b.neon);
    return result;
}

Vector4 Vector4::multiply(const Vector4& a, const Vector4& b) {
    Vector4 result;
    result.neon = vmulq_f32(a.neon, b.neon);
    return result;
}

Vector4 Vector4::scale(const Vector4& a, float scalar) {
    Vector4 result;
    float32x4_t scale_vec = vdupq_n_f32(scalar);
    result.neon = vmulq_f32(a.neon, scale_vec);
    return result;
}

float Vector4::dot(const Vector4& a, const Vector4& b) {
    float32x4_t mul = vmulq_f32(a.neon, b.neon);
    float32x2_t sum2 = vadd_f32(vget_high_f32(mul), vget_low_f32(mul));
    return vget_lane_f32(vpadd_f32(sum2, sum2), 0);
}

Vector4 Vector4::cross(const Vector4& a, const Vector4& b) {
    return Vector4(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0.0f
    );
}

Vector4 Vector4::normalize(const Vector4& a) {
    float len = length(a);
    if (len > 0.0f) {
        float invLen = 1.0f / len;
        return scale(a, invLen);
    }
    return a;
}

float Vector4::length(const Vector4& a) {
    return FastMath::sqrt(dot(a, a));
}

float Vector4::lengthSquared(const Vector4& a) {
    return dot(a, a);
}

Vector4 Vector4::fastNormalize(const Vector4& a) {
    float len = fastLength(a);
    if (len > 0.0f) {
        float invLen = FastMath::fastInverseSqrt(len * len);
        return scale(a, invLen);
    }
    return a;
}

float Vector4::fastLength(const Vector4& a) {
    return FastMath::fastSqrt(dot(a, a));
}

float Vector4::fastInverseLength(const Vector4& a) {
    return FastMath::fastInverseSqrt(dot(a, a));
}

// Vector3 实现
Vector3 Vector3::add(const Vector3& a, const Vector3& b) {
    return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 Vector3::subtract(const Vector3& a, const Vector3& b) {
    return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 Vector3::multiply(const Vector3& a, const Vector3& b) {
    return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3 Vector3::scale(const Vector3& a, float scalar) {
    return Vector3(a.x * scalar, a.y * scalar, a.z * scalar);
}

float Vector3::dot(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 Vector3::cross(const Vector3& a, const Vector3& b) {
    return Vector3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

Vector3 Vector3::normalize(const Vector3& a) {
    float len = length(a);
    if (len > 0.0f) {
        float invLen = 1.0f / len;
        return scale(a, invLen);
    }
    return a;
}

float Vector3::length(const Vector3& a) {
    return FastMath::sqrt(dot(a, a));
}

float Vector3::lengthSquared(const Vector3& a) {
    return dot(a, a);
}

Vector3 Vector3::fastNormalize(const Vector3& a) {
    float lenSq = lengthSquared(a);
    if (lenSq > 0.0f) {
        float invLen = FastMath::fastInverseSqrt(lenSq);
        return scale(a, invLen);
    }
    return a;
}

float Vector3::fastLength(const Vector3& a) {
    return FastMath::fastSqrt(dot(a, a));
}

// Vector2 实现
Vector2 Vector2::add(const Vector2& a, const Vector2& b) {
    return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 Vector2::subtract(const Vector2& a, const Vector2& b) {
    return Vector2(a.x - b.x, a.y - b.y);
}

Vector2 Vector2::multiply(const Vector2& a, const Vector2& b) {
    return Vector2(a.x * b.x, a.y * b.y);
}

Vector2 Vector2::scale(const Vector2& a, float scalar) {
    return Vector2(a.x * scalar, a.y * scalar);
}

float Vector2::dot(const Vector2& a, const Vector2& b) {
    return a.x * b.x + a.y * b.y;
}

float Vector2::length(const Vector2& a) {
    return FastMath::sqrt(dot(a, a));
}

float Vector2::lengthSquared(const Vector2& a) {
    return dot(a, a);
}

Vector2 Vector2::normalize(const Vector2& a) {
    float len = length(a);
    if (len > 0.0f) {
        float invLen = 1.0f / len;
        return scale(a, invLen);
    }
    return a;
}

// BatchVectorOperations 实现
void BatchVectorOperations::addVectors4(const Vector4* a, const Vector4* b, Vector4* result, size_t count) {
    // 使用NEON进行批量加法
    for (size_t i = 0; i < count; i += 4) {
        size_t remaining = count - i;
        if (remaining >= 4) {
            // 一次处理4个向量
            float32x4x4_t a4 = vld4q_f32(&a[i].x);
            float32x4x4_t b4 = vld4q_f32(&b[i].x);
            
            float32x4x4_t result4;
            result4.val[0] = vaddq_f32(a4.val[0], b4.val[0]);
            result4.val[1] = vaddq_f32(a4.val[1], b4.val[1]);
            result4.val[2] = vaddq_f32(a4.val[2], b4.val[2]);
            result4.val[3] = vaddq_f32(a4.val[3], b4.val[3]);
            
            vst4q_f32(&result[i].x, result4);
        } else {
            // 处理剩余向量
            for (size_t j = i; j < count; ++j) {
                result[j] = Vector4::add(a[j], b[j]);
            }
        }
    }
}

void BatchVectorOperations::addVectors3(const Vector3* a, const Vector3* b, Vector3* result, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        result[i] = Vector3::add(a[i], b[i]);
    }
}

void BatchVectorOperations::scaleVectors4(const Vector4* vectors, float scalar, Vector4* result, size_t count) {
    float32x4_t scale_vec = vdupq_n_f32(scalar);
    for (size_t i = 0; i < count; ++i) {
        result[i].neon = vmulq_f32(vectors[i].neon, scale_vec);
    }
}

void BatchVectorOperations::scaleVectors3(const Vector3* vectors, float scalar, Vector3* result, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        result[i] = Vector3::scale(vectors[i], scalar);
    }
}

void BatchVectorOperations::dotProducts4(const Vector4* a, const Vector4* b, float* results, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        results[i] = Vector4::dot(a[i], b[i]);
    }
}

void BatchVectorOperations::dotProducts3(const Vector3* a, const Vector3* b, float* results, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        results[i] = Vector3::dot(a[i], b[i]);
    }
}

void BatchVectorOperations::normalizeVectors4(Vector4* vectors, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        vectors[i] = Vector4::normalize(vectors[i]);
    }
}

void BatchVectorOperations::normalizeVectors3(Vector3* vectors, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        vectors[i] = Vector3::normalize(vectors[i]);
    }
}

void BatchVectorOperations::fastNormalizeVectors4(Vector4* vectors, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        vectors[i] = Vector4::fastNormalize(vectors[i]);
    }
}

void BatchVectorOperations::fastNormalizeVectors3(Vector3* vectors, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        vectors[i] = Vector3::fastNormalize(vectors[i]);
    }
}

} // namespace OptimizedMath