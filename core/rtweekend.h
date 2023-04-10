#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cstdlib>
#include <cmath>
#include <limits>
#include <memory>
#include"geometry.h"

//////////////////////////////////////////////////

//usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;
//////////////////////////////////////////////////

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385f;

//////////////////////////////////////////////////

//utility functions 

inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.;
}

//return a randow real in [0,1)
inline double random_float(){
    return rand() / (RAND_MAX + 1.0);
}

//return a random real in [min,max)
inline double random_float(float min, float max){
    return min + (max-min)*random_float();
}
// returns a random integer in [min,max]
inline int random_int(int min, int max){
    return static_cast<int>(random_float(min, max+1));
}

//which clamps the value x to the range [min,max]
inline float clamp(float x, float min, float max){
    if(x<min) return min;
    if(x>max) return max;
    return x;
}
//rejection method

inline  Vec3f random_v3f(){
        return Vec3f(random_float(),random_float(),random_float());
    }

inline  Vec3f random_v3f(float min, float max){
    return Vec3f(random_float(min,max),random_float(min,max),random_float(min,max));
}

inline Vec3f random_in_unit_sphere(){
    while (true)
    {
        auto p = random_v3f(-1,1);
        if(p.norm() >= 1) continue;
        return p;
    }
}

inline Vec3f random_unit_vector(){
    return random_in_unit_sphere().normalize();
    
}

inline Vec3f random_int_hemisphere(const Vec3f &normal){
    Vec3f in_unit_sphere = random_in_unit_sphere();
    if(in_unit_sphere*normal > 0.) // in the same hemisphere as the normal
        return in_unit_sphere;
    else return -in_unit_sphere;
}

inline Vec3f random_in_unit_disk(){
    while (true)
    {
        auto p = Vec3f(random_float(-1,1), random_float(-1,1), 0);
        if(p.norm() >= 1) continue;
        return p;
    }
    
}

inline Vec3f random_cosine_direction() {
    auto r1 = random_float();
    auto r2 = random_float();
    auto z = sqrt(1-r2);

    auto phi = 2*pi*r1;
    auto x = cos(phi)*sqrt(r2);
    auto y = sin(phi)*sqrt(r2);

    return Vec3f(x, y, z);
}

inline Vec3f random_to_sphere(float radius, float distance_squared){
    auto r1 = random_float();
    auto r2 = random_float();
    auto z = 1 + r2*(sqrt(1-radius*radius/distance_squared) - 1);

    auto phi = 2*pi*r1;
    auto x = cos(phi)*sqrt(1-z*z);
    auto y = sin(phi)*sqrt(1-z*z);

    return Vec3f(x,y,z);
}

inline Vec3f reflect(const Vec3f& l, const Vec3f& n){
    return l - (l*n)*n*2.f;    // light: view to hit_point
}

inline Vec3f refract(const Vec3f& uv, const Vec3f& n, float etai_over_etat){
    //uv: the incident light
    auto cos_theta = fmin(-uv*n, 1.0);
    Vec3f r_out_perp = etai_over_etat*(uv+cos_theta*n);
    Vec3f r_out_parallel = -sqrtf(fabs(1.0-r_out_perp*r_out_perp))*n;
    return r_out_perp+r_out_parallel;
}

inline color attenuation_ray_color(const color& attenuation, const color& ray){
    return color(attenuation.x*ray.x, attenuation.y*ray.y, attenuation.z*ray.z);
}
//////////////////////////////////////////////////

// Common Headers

#include "ray.h"
#include "geometry.h"

#endif