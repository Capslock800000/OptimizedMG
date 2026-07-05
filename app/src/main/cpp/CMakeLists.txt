
cmake_minimum_required(VERSION 3.18.1)

project(optimizedmg)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_library(log-lib log)
find_library(android-lib android)
find_library(egl-lib EGL)
find_library(glesv3-lib GLESv3)

set(MG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/MobileGlues-cpp)

# Root project sources
set(ROOT_SOURCES
    main.cpp
    native-lib.cpp
    PerformanceMonitor.cpp
    QuaternionMath.cpp
    math/VectorMath.cpp
    math/MatrixMath.cpp
    math/FastMath.cpp
    mobileglues/OptimizedRenderer.cpp
    mobileglues/ShaderManager.cpp
    mobileglues/TextureCache.cpp
)

# MobileGlues runtime sources (merged in; exclude the dependency-heavy shader transpiler entry)
file(GLOB_RECURSE MOBILEGLUES_SOURCES CONFIGURE_DEPENDS
    ${MG_ROOT}/gl/*.cpp
    ${MG_ROOT}/egl/*.cpp
    ${MG_ROOT}/gles/*.cpp
    ${MG_ROOT}/glx/*.cpp
    ${MG_ROOT}/config/*.cpp
)

# Exclude source files that require missing/optional third-party build products.
list(FILTER MOBILEGLUES_SOURCES EXCLUDE REGEX ".*/gl/glsl/glsl_for_es\\.cpp$")
list(FILTER MOBILEGLUES_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
list(FILTER MOBILEGLUES_SOURCES EXCLUDE REGEX ".*/init\\.cpp$")
list(FILTER MOBILEGLUES_SOURCES EXCLUDE REGEX ".*/include/SPIRV/.*\\.cpp$")

add_library(optimizedmg SHARED
    ${ROOT_SOURCES}
    ${MOBILEGLUES_SOURCES}
    ${MG_ROOT}/gl/glsl/glsl_for_es_stub.cpp
)

target_include_directories(optimizedmg PRIVATE
    # FSR1 requires GLM headers
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/math
    ${CMAKE_CURRENT_SOURCE_DIR}/mobileglues

    ${MG_ROOT}
    ${MG_ROOT}/gl
    ${MG_ROOT}/gl/ExtWrappers
    ${MG_ROOT}/gl/FSR1
    ${MG_ROOT}/gl/glsl
    ${MG_ROOT}/egl
    ${MG_ROOT}/gles
    ${MG_ROOT}/glx
    ${MG_ROOT}/config
    ${MG_ROOT}/include
    ${MG_ROOT}/include/SPIRV
    ${MG_ROOT}/include/glslang
    ${MG_ROOT}/include/spirv_cross
    ${MG_ROOT}/include/glm
    ${MG_ROOT}/include
)

target_compile_options(optimizedmg PRIVATE
    -O3
    -ffast-math
    -fvisibility=hidden
)

target_link_libraries(optimizedmg
    ${android-lib}
    ${log-lib}
    ${egl-lib}
    ${glesv3-lib}
)


target_sources(optimizedmg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/MobileGlues-cpp/config/cJSON.c
)
