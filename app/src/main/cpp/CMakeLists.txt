cmake_minimum_required(VERSION 3.18.1)

project("optimizedmg")

# 设置极致优化的编译选项
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 优化标志
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -ffast-math -march=armv8-a+simd -mfpu=neon -ftree-vectorize -funsafe-math-optimizations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -march=armv8-a+simd -mfpu=neon -ftree-vectorize -funsafe-math-optimizations")

# 查找必要的库
find_library(log-lib log)
find_library(android-lib android)
find_library(egl-lib EGL)
find_library(glesv2-lib GLESv2)

# 主库
add_library(
    optimizedmg
    SHARED
    main.cpp
    math/VectorMath.cpp
    math/MatrixMath.cpp
    math/QuaternionMath.cpp
    math/FastMath.cpp
    mobileglues/OptimizedRenderer.cpp
    mobileglues/ShaderManager.cpp
    mobileglues/TextureCache.cpp
)

# 链接库
target_link_libraries(
    optimizedmg
    ${log-lib}
    ${android-lib}
    ${egl-lib}
    ${glesv2-lib}
)

# 包含目录
target_include_directories(optimizedmg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})