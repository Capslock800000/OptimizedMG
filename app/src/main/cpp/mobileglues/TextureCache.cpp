#include "TextureCache.h"

GLuint TextureCache::get(const std::string& key) {
    auto it = m_index.find(key);
    if (it == m_index.end()) return 0;

    m_lru.splice(m_lru.begin(), m_lru, it->second);
    return it->second->second;
}

void TextureCache::put(const std::string& key, GLuint texture) {
    auto it = m_index.find(key);
    if (it != m_index.end()) {
        if (it->second->second != texture && it->second->second) {
            GLuint oldTex = it->second->second;
            glDeleteTextures(1, &oldTex);
        }
        it->second->second = texture;
        m_lru.splice(m_lru.begin(), m_lru, it->second);
        return;
    }

    m_lru.emplace_front(key, texture);
    m_index.emplace(key, m_lru.begin());
}

void TextureCache::clear() {
    if (!m_lru.empty()) {
        for (const auto& kv : m_lru) {
            if (kv.second) {
                glDeleteTextures(1, &kv.second);
            }
        }
    }
    m_lru.clear();
    m_index.clear();
}
