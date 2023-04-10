#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include"rtweekend.h"
#include"hittable.h"
#include"aabb.h"

class moving_sphere : public hittable
{
public:
    moving_sphere(){}
    moving_sphere(point3 cen0, point3 cen1, float _time0, float _time1, float r,shared_ptr<material> m)
    :center0(cen0),center1(cen1),time0(_time0),time1(_time1),radius(r),mat_ptr(m)
    {}
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
    virtual bool bounding_box(float _time0, float _time1, aabb& output_box) const override;
    
    point3 center(float time) const;

public:
    point3 center0,center1;
    float time0,time1;
    float radius;
    shared_ptr<material> mat_ptr;

};

point3 moving_sphere::center(float time) const{
    return center0 + (time-time0)/(time1-time0)*(center1-center0);
}

bool moving_sphere::hit(const ray &r, float t_min, float t_max, hit_record& rec) const {
    Vec3f oc = r.orig - center(r.time());
    auto a = r.dir*r.dir;
    // auto b = 2.*r.dir*oc;
    auto half_b = r.dir*oc;
    auto c = oc*oc - radius*radius;
    // auto discriminant = b*b -4*a*c;
    auto discriminant = half_b*half_b -a*c;
    if(discriminant<0) return false;

    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {//root is negative or bigger than max,enter if;
        
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    // rec.normal = (rec.p - center)/radius;
    Vec3f outward_normal = (rec.p - center(r.time()))/radius; // already normalize
    rec.set_face_normal(r,outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool moving_sphere::bounding_box(float _time0, float _time1, aabb& output_box) const {
    aabb box0(
        center(_time0) - Vec3f(radius, radius, radius),
        center(_time0) + Vec3f(radius, radius, radius)
    );
    aabb box1(
        center(_time1) - Vec3f(radius, radius, radius),
        center(_time1) + Vec3f(radius, radius, radius)
    );
    output_box = surrounding_box(box0,box1);

    return true;
}

#endif