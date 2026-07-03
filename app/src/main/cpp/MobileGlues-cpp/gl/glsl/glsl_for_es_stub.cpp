
// Auto-generated compatibility stub for buildability.
#include "glsl_for_es.h"
#include <string>

namespace {
int ParseVersion(const char* glsl_code) {
    if (!glsl_code) return 0;
    const char* p = glsl_code;
    while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') ++p;
    if (std::string_view(p).rfind("#version", 0) != 0) return 0;
    int ver = 0;
    p += 8;
    while (*p == ' ' || *p == '\t') ++p;
    while (*p >= '0' && *p <= '9') {
        ver = ver * 10 + (*p - '0');
        ++p;
    }
    return ver;
}
} // namespace

std::string GLSLtoGLSLES(const char* glsl_code, GLenum, uint, uint, int& return_code) {
    return_code = 0;
    return glsl_code ? std::string(glsl_code) : std::string();
}

std::string GLSLtoGLSLES_1(const char* glsl_code, GLenum glsl_type, uint esversion, int& return_code) {
    return GLSLtoGLSLES(glsl_code, glsl_type, esversion, 0, return_code);
}

std::string GLSLtoGLSLES_2(const char* glsl_code, GLenum glsl_type, uint essl_version, int& return_code) {
    return GLSLtoGLSLES(glsl_code, glsl_type, essl_version, 0, return_code);
}

int getGLSLVersion(const char* glsl_code) {
    return ParseVersion(glsl_code);
}
