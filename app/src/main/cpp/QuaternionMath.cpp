
#include <cmath>

struct Quaternion {
    float x,y,z,w;
};

class QuaternionMath {
public:
    static Quaternion identity(){ return {0,0,0,1}; }

    static Quaternion fromAxisAngle(float x,float y,float z,float a){
        float s = sinf(a*0.5f);
        return {x*s,y*s,z*s,cosf(a*0.5f)};
    }

    static Quaternion multiply(const Quaternion&a,const Quaternion&b){
        return {
            a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
            a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
            a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
            a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
        };
    }
};
