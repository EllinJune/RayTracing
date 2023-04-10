#ifndef AARECT_H
#define AARECT_H

#include"rtweekend.h"
#include"hittable.h"

class xy_rect : public hittable
{
public:
    xy_rect(){}
    xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, shared_ptr<material> mat) 
    : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {}

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override{
        // The bounding box must have non-zero width in each dimension, so pad the Z
        // dimension a small amount.
        output_box = aabb(point3(x0,y0,k-1e-4), point3(x1,y1,k+1e-4));
        return true;
    }
 
public:
    shared_ptr<material> mp;
    float x0,x1,y0,y1,k;// k  equals z
};

class xz_rect : public hittable {
    public:
        xz_rect() {}

        xz_rect(double _x0, double _x1, double _z0, double _z1, double _k,
            shared_ptr<material> mat)
            : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

        virtual bool bounding_box(float time0, float time1, aabb& output_box) const override {
            // The bounding box must have non-zero width in each dimension, so pad the Y
            // dimension a small amount.
            output_box = aabb(point3(x0,k-0.0001,z0), point3(x1, k+0.0001, z1));
            return true;
        }

        virtual float pdf_value(const point3& origin, const Vec3f& direction) const override{
            hit_record rec;
            if(!this->hit(ray(origin, direction), 1e-3, infinity, rec))
                return 0;

            auto area = (x1-x0)*(z1-z0);
            auto distance_squared = rec.t*rec.t * direction*direction;
            auto cosine = fabs(direction*rec.normal / direction.norm());

            return distance_squared / (cosine*area);
        }
        virtual Vec3f random(const Vec3f& origin) const override{
            auto random_point = point3(random_float(x0,x1), k, random_float(z0,z1));
            return random_point - origin;
        }

    public:
        shared_ptr<material> mp;
        float x0, x1, z0, z1, k; // k equals y
};

class yz_rect : public hittable {
    public:
        yz_rect() {}

        yz_rect(double _y0, double _y1, double _z0, double _z1, double _k,
            shared_ptr<material> mat)
            : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;

        virtual bool bounding_box(float time0, float time1, aabb& output_box) const override {
            // The bounding box must have non-zero width in each dimension, so pad the X
            // dimension a small amount.
            output_box = aabb(point3(k-0.0001, y0, z0), point3(k+0.0001, y1, z1));
            return true;
        }

    public:
        shared_ptr<material> mp;
        float y0, y1, z0, z1, k;
};

bool xy_rect::hit(const ray &r, float t_min, float t_max, hit_record& rec) const{
    auto t = (k-r.orig.z) / r.dir.z;
    if(t < t_min || t > t_max)
        return false;
    auto x = r.orig.x + t*r.dir.x;
    auto y = r.orig.y + t*r.dir.y;
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;
    rec.u = (x-x0)/(x1-x0);
    rec.v = (y-y0)/(y1-y0);
    rec.t = t;
    auto outward_normal = Vec3f(0,0,1);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(t);
    return true;
}

bool xz_rect::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    auto t = (k-r.orig.y) / r.dir.y;
    if (t < t_min || t > t_max)
        return false;
    auto x = r.orig.x + t*r.dir.x;
    auto z = r.orig.z + t*r.dir.z;
    if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;
    rec.u = (x-x0)/(x1-x0);
    rec.v = (z-z0)/(z1-z0);
    rec.t = t;
    auto outward_normal = Vec3f(0, 1, 0);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(t);
    return true;
}

bool yz_rect::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    auto t = (k-r.orig.x) / r.dir.x;
    if (t < t_min || t > t_max)
        return false;
    auto y = r.orig.y + t*r.dir.y;
    auto z = r.orig.z + t*r.dir.z;
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;
    rec.u = (y-y0)/(y1-y0);
    rec.v = (z-z0)/(z1-z0);
    rec.t = t;
    auto outward_normal = Vec3f(1, 0, 0);
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(t);
    return true;
}

#endif