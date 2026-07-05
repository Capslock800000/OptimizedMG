#pragma once
#include <cstddef>

namespace glm {
struct vec2 {
    float x, y;
    constexpr vec2() : x(0.0f), y(0.0f) {}
    constexpr vec2(float x_, float y_) : x(x_), y(y_) {}
};

struct vec3 {
    float x, y, z;
    constexpr vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

struct vec4 {
    float x, y, z, w;
    constexpr vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    constexpr vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
};
} // namespace glm
