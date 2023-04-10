#ifndef RAY_H
#define RAY_H

#include"geometry.h"

class ray{
    public:
        ray(){}
        ray(const point3& o, const Vec3f &d, float time=0.f):orig(o), dir(d), tm(time){}

        point3 at(float t) const{
            return orig+t*dir;
        }
        Vec3f direction() const { return dir; }
        float time() const { return tm;}

    
    public:
        point3 orig;
        Vec3f dir;
        float tm;

};


#endif