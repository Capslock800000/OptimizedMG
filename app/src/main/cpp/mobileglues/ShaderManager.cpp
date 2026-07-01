
#include <GLES3/gl3.h>
#include <unordered_map>
#include <string>

class ShaderManager {
    std::unordered_map<std::string,GLuint> cache;
public:
    GLuint compile(GLenum t,const char* src){
        GLuint s=glCreateShader(t);
        glShaderSource(s,1,&src,nullptr);
        glCompileShader(s);
        return s;
    }

    GLuint load(std::string v,std::string f){
        std::string k=v+f;
        if(cache.count(k)) return cache[k];

        GLuint vs=compile(GL_VERTEX_SHADER,v.c_str());
        GLuint fs=compile(GL_FRAGMENT_SHADER,f.c_str());

        GLuint p=glCreateProgram();
        glAttachShader(p,vs);
        glAttachShader(p,fs);
        glLinkProgram(p);

        glDeleteShader(vs);
        glDeleteShader(fs);

        cache[k]=p;
        return p;
    }
};
