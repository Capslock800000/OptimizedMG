#pragma once
#include <GLES3/gl3.h>
#include <string>
#include <unordered_map>
#include <list>

class TextureCache {
public:
    TextureCache() = default;
    ~TextureCache() { clear(); }

    GLuint get(const std::string& key);
    void put(const std::string& key, GLuint texture);
    void clear();

private:
    std::unordered_map<std::string, std::list<std::pair<std::string, GLuint>>::iterator> m_index;
    std::list<std::pair<std::string, GLuint>> m_lru;
};
