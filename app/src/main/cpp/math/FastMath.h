#pragma once

#include <cmath>
#include <arm_neon.h>

namespace OptimizedMath {

class FastMath {
public:
    // 快速平方根 - 使用位操作和牛顿迭代法
    static float fastSqrt(float x);
    
    // 快速反平方根 - 著名的Quake III算法
    static float fastInverseSqrt(float x);
    
    // 快速正弦和余弦 - 使用查表法和多项式近似
    static float fastSin(float x);
    static float fastCos(float x);
    static void fastSinCos(float x, float& sin, float& cos);
    
    // 快速指数函数
    static float fastExp(float x);
    
    // 快速对数函数
    static float fastLog(float x);
    
    // 快速atan2
    static float fastAtan2(float y, float x);
    
    // 标准数学函数的包装（在需要精确性时使用）
    static float sqrt(float x) { return ::sqrtf(x); }
    static float sin(float x) { return ::sinf(x); }
    static float cos(float x) { return ::cosf(x); }
    static float tan(float x) { return ::tanf(x); }
    static float exp(float x) { return ::expf(x); }
    static float log(float x) { return ::logf(x); }
    static float atan2(float y, float x) { return ::atan2f(y, x); }
    
    // 批量运算
    static void fastSqrtArray(const float* input, float* output, size_t count);
    static void fastInverseSqrtArray(const float* input, float* output, size_t count);
    static void fastSinArray(const float* input, float* output, size_t count);
    static void fastCosArray(const float* input, float* output, size_t count);
    
    // 常量
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float TWO_PI = 2.0f * PI;
    static constexpr float HALF_PI = 0.5f * PI;
    static constexpr float INV_PI = 1.0f / PI;
    static constexpr float INV_TWO_PI = 1.0f / TWO_PI;
    
private:
    // 查表法用的查找表
    static const float SIN_TABLE[1025];
    static const float EXP_TABLE[256];
    
    // 辅助函数
    static float reduceAngle(float x);
    static int32_t floatToInt(float x);
    static float intToFloat(int32_t x);
};

} // namespace OptimizedMath