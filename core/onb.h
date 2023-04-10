#ifndef ONB_H
#define ONB_H

#include"rtweekend.h"


class onb
{
public:
    onb(){}
    ~onb(){}
    inline Vec3f operator[](int i) const {  return axis[i];}

    Vec3f u() const { return axis[0];}
    Vec3f v() const { return axis[1];}
    Vec3f w() const { return axis[2];}

    Vec3f local(float a, float b, float c) const{
        return a*u() + b*v() + c*w();
    }
    Vec3f local(const Vec3f& a) const{
        return a.x*u() + a.y*v() + a.z*w();
    }

    void build_from_w(const Vec3f& n);

public:
    Vec3f axis[3];
    
};

void onb::build_from_w(const Vec3f& n){
    axis[2] = Vec3f(n).normalize();
    Vec3f a = (fabs(w().x) > 0.9 ) ? Vec3f(0,1,0) : Vec3f(1,0,0);//选择一个任意的a作为初始向量,默认为x轴，为了避免与n平行，所以进行判断
    axis[1] = cross(w(),a).normalize();
    axis[0] = cross(w(), v());
}



#endif