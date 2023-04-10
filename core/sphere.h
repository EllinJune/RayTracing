#ifndef SPHERE_H
#define SPHERE_H

#include"hittable.h"
#include"geometry.h"
#include"aabb.h"
#include"onb.h"

class sphere : public hittable
{
public:
    sphere() {}
    sphere(point3 c, double r, shared_ptr<material> m):center(c),radius(r),mat_ptr(m){};

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const override;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;
    virtual float pdf_value(const point3& origin, const Vec3f& v) const override;
    virtual Vec3f random(const Vec3f& origin) const override;
private:
    static void get_sphere_uv(const point3& p, float& u, float& v){
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1. u = phi / 2pi, phi:[0,2pi]
        // v: returned value [0,1] of angle from Y=-1 to Y=+1. v = theta / pi, theta:[0,pi]
        // y = - cos(theta) x = - cos(phi)sin(theta) z = sin(phi)sin(theta)
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.y);
        auto phi = atan2(-p.z, p.x) + pi;

        u = phi/(2*pi);
        v = theta/pi;
    }

public:
    point3 center;
    float radius;
    shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray &r, float t_min, float t_max, hit_record& rec) const {
    Vec3f oc = r.orig - center;
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
    Vec3f outward_normal = (rec.p - center)/radius; // already normalize
    rec.set_face_normal(r,outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);//update hit record uv coords
    rec.mat_ptr = mat_ptr;

    return true;
}


bool sphere::bounding_box(float time0, float time1, aabb& output_box) const {
    output_box = aabb(
        center - Vec3f(radius,radius,radius),
        center + Vec3f(radius,radius,radius)
    );
    return true;

}

float sphere::pdf_value(const point3& origin, const Vec3f& v) const{
    hit_record rec;
    if(!this->hit(ray(origin,v),1e-3,infinity, rec))
        return 0;
    auto cos_theta_max = sqrt(1-radius*radius/(center-origin).length_squared());
    auto solid_angle = 2*pi*(1-cos_theta_max);

    return 1 / solid_angle;
}

Vec3f sphere::random(const Vec3f& origin) const{
    Vec3f direction = center - origin;
    auto distance_squared = direction.length_squared();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));

}


#endif
