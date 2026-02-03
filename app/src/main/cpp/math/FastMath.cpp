#include "FastMath.h"
#include <cstdint>

namespace OptimizedMath {

// 正弦查找表 (0 到 2PI，1024个采样点)
const float FastMath::SIN_TABLE[1025] = {
    #include "sin_table.inl" // 实际项目中需要生成这个文件
};

// 指数查找表 (-8 到 8，256个采样点)
const float FastMath::EXP_TABLE[256] = {
    #include "exp_table.inl" // 实际项目中需要生成这个文件
};

float FastMath::fastSqrt(float x) {
    if (x <= 0.0f) return 0.0f;
    
    // 使用位操作进行初始估计
    int32_t i = floatToInt(x);
    i = 0x5f3759df - (i >> 1);
    float y = intToFloat(i);
    
    // 一次牛顿迭代提高精度
    y = y * (1.5f - 0.5f * x * y * y);
    return x * y; // 修正结果
}

float FastMath::fastInverseSqrt(float x) {
    if (x <= 0.0f) return 0.0f;
    
    // 著名的Quake III反平方根算法
    int32_t i = floatToInt(x);
    i = 0x5f3759df - (i >> 1);
    float y = intToFloat(i);
    
    // 一次牛顿迭代提高精度
    y = y * (1.5f - 0.5f * x * y * y);
    return y;
}

float FastMath::fastSin(float x) {
    // 将角度规整到 [0, 2PI)
    x = reduceAngle(x);
    
    // 使用查找表
    float index = x * (1024.0f / TWO_PI);
    int i0 = static_cast<int>(index);
    int i1 = (i0 + 1) & 1023; // 循环索引
    float t = index - i0;
    
    // 线性插值
    return SIN_TABLE[i0] + t * (SIN_TABLE[i1] - SIN_TABLE[i0]);
}

float FastMath::fastCos(float x) {
    return fastSin(x + HALF_PI);
}

void FastMath::fastSinCos(float x, float& sin, float& cos) {
    x = reduceAngle(x);
    
    float index = x * (1024.0f / TWO_PI);
    int i0 = static_cast<int>(index);
    int i1 = (i0 + 1) & 1023;
    float t = index - i0;
    
    sin = SIN_TABLE[i0] + t * (SIN_TABLE[i1] - SIN_TABLE[i0]);
    cos = SIN_TABLE[(i0 + 256) & 1023] + t * (SIN_TABLE[(i1 + 256) & 1023] - SIN_TABLE[(i0 + 256) & 1023]);
}

float FastMath::fastExp(float x) {
    // 限制输入范围
    if (x < -8.0f) return 0.0f;
    if (x > 8.0f) return ::expf(x); // 回退到标准函数
    
    // 使用查找表
    float index = (x + 8.0f) * (255.0f / 16.0f);
    int i0 = static_cast<int>(index);
    int i1 = i0 + 1;
    if (i1 > 255) i1 = 255;
    float t = index - i0;
    
    return EXP_TABLE[i0] + t * (EXP_TABLE[i1] - EXP_TABLE[i0]);
}

float FastMath::fastLog(float x) {
    if (x <= 0.0f) return -INFINITY;
    
    // 使用对数恒等式和对数多项式近似
    int32_t i = floatToInt(x);
    int32_t exponent = ((i >> 23) & 0xFF) - 127;
    float mantissa = intToFloat((i & 0x007FFFFF) | 0x3F800000);
    
    // 对尾数使用多项式近似 log(1 + m)
    float m = mantissa - 1.0f;
    float log_m = m - 0.5f * m * m + 0.333333f * m * m * m - 0.25f * m * m * m * m;
    
    return exponent * 0.6931471805599453f + log_m; // ln(2) ≈ 0.693147
}

float FastMath::fastAtan2(float y, float x) {
    if (x == 0.0f) {
        if (y > 0.0f) return HALF_PI;
        if (y < 0.0f) return -HALF_PI;
        return 0.0f;
    }
    
    float abs_x = x < 0 ? -x : x;
    float abs_y = y < 0 ? -y : y;
    
    // 使用有理函数近似
    float a = (abs_x < abs_y) ? abs_x / abs_y : abs_y / abs_x;
    float s = a * a;
    float r = ((-0.0464964749f * s + 0.15931422f) * s - 0.327622764f) * s * a + a;
    
    if (abs_y > abs_x) r = HALF_PI - r;
    if (x < 0) r = PI - r;
    if (y < 0) r = -r;
    
    return r;
}

void FastMath::fastSqrtArray(const float* input, float* output, size_t count) {
    for (size_t i = 0; i < count; i += 4) {
        size_t remaining = count - i;
        if (remaining >= 4) {
            // 使用NEON一次处理4个值
            float32x4_t vec = vld1q_f32(&input[i]);
            float32x4_t sqrt_vec = vmulq_f32(vec, vrsqrteq_f32(vec));
            vst1q_f32(&output[i], sqrt_vec);
        } else {
            // 处理剩余元素
            for (size_t j = i; j < count; ++j) {
                output[j] = fastSqrt(input[j]);
            }
        }
    }
}

void FastMath::fastInverseSqrtArray(const float* input, float* output, size_t count) {
    for (size_t i = 0; i < count; i += 4) {
        size_t remaining = count - i;
        if (remaining >= 4) {
            float32x4_t vec = vld1q_f32(&input[i]);
            float32x4_t inv_sqrt_vec = vrsqrteq_f32(vec);
            vst1q_f32(&output[i], inv_sqrt_vec);
        } else {
            for (size_t j = i; j < count; ++j) {
                output[j] = fastInverseSqrt(input[j]);
            }
        }
    }
}

void FastMath::fastSinArray(const float* input, float* output, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        output[i] = fastSin(input[i]);
    }
}

void FastMath::fastCosArray(const float* input, float* output, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        output[i] = fastCos(input[i]);
    }
}

float FastMath::reduceAngle(float x) {
    // 使用fmodf规整角度到 [0, 2PI)
    x = ::fmodf(x, TWO_PI);
    if (x < 0.0f) x += TWO_PI;
    return x;
}

int32_t FastMath::floatToInt(float x) {
    union {
        float f;
        int32_t i;
    } u;
    u.f = x;
    return u.i;
}

float FastMath::intToFloat(int32_t x) {
    union {
        float f;
        int32_t i;
    } u;
    u.i = x;
    return u.f;
}

} // namespace OptimizedMath