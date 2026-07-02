#include <cmath>

struct Quaternion {
    float x, y, z, w;
};

class QuaternionMath {
public:
    static inline Quaternion identity() noexcept { return {0.0f, 0.0f, 0.0f, 1.0f}; }

    static inline Quaternion normalize(const Quaternion& q) noexcept {
        const float lenSq = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
        if (lenSq <= 0.0f) return identity();
        const float invLen = 1.0f / std::sqrt(lenSq);
        return {q.x * invLen, q.y * invLen, q.z * invLen, q.w * invLen};
    }

    static inline Quaternion conjugate(const Quaternion& q) noexcept {
        return {-q.x, -q.y, -q.z, q.w};
    }

    static inline Quaternion fromAxisAngle(float x, float y, float z, float angleRad) noexcept {
        const float half = angleRad * 0.5f;
        const float s = ::sinf(half);
        const float c = ::cosf(half);
        return normalize({x * s, y * s, z * s, c});
    }

    static inline Quaternion multiply(const Quaternion& a, const Quaternion& b) noexcept {
        return {
            a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
            a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
            a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
            a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
        };
    }
};
