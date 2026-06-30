
#include <GLES3/gl3.h>
#include <unordered_map>
#include <string>

class TextureCache {
    std::unordered_map<std::string,GLuint> cache;
public:
    GLuint get(const std::string&k){
        return cache.count(k)?cache[k]:0;
    }
    void put(const std::string&k,GLuint t){
        cache[k]=t;
    }
};
