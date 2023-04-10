#ifndef HITTABLE_H
#define HITTABLE_H

#include"ray.h"
#include"rtweekend.h"
#include"aabb.h"

//提醒编译器指针指向一个类
class material;

//record hit point and normal
struct hit_record
{
    point3 p;
    Vec3f normal;
    shared_ptr<material> mat_ptr;
    float t;
    float u;// u,v surface coordinates
    float v;
    bool front_face;//front_face表示是否朝外，而normal一定朝外

    inline void set_face_normal(const ray& r, const Vec3f& outward_normal){
        front_face = r.dir*outward_normal < 0; // front = true, 光线从外射到表面，false,从内射往往
        normal = front_face? outward_normal : -outward_normal;
    }
    
};

class hittable
{
public:
    virtual bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const = 0;
    virtual float pdf_value(const point3& origin, const Vec3f& v) const {
        return 0.f;
    }
    virtual Vec3f random(const Vec3f& origin) const {
        return Vec3f(1,0,0);
    }
};

class translate : public hittable
{
public:
    translate(shared_ptr<hittable> p, const Vec3f& displacement) : ptr(p), offset(displacement) {}

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;

public:
    shared_ptr<hittable> ptr;
    Vec3f offset;

};

bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    //key1: the ray have been moved to hit at box's orign position
    ray moved_r(r.orig - offset, r.dir, r.time());

    if(!ptr->hit(moved_r, t_min, t_max, rec))
        return false;
    
    //key2: recover by plus offset means generate image at the box's translated position
    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}

bool translate::bounding_box(float time0, float time1, aabb& output_box) const {
    if(!ptr->bounding_box(time0,time1,output_box))
        return false;
    
    output_box = aabb(
        output_box.min() + offset,
        output_box.max() + offset
    );

    return true;
}

class rotate_y : public hittable
{
public:
    rotate_y(shared_ptr<hittable> p, float angle);

    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override{
        output_box = bbox;
        return hasbox;
    }

public:
    shared_ptr<hittable> ptr;
    float sin_theta;
    float cos_theta;
    bool hasbox;
    aabb bbox;
};

rotate_y::rotate_y(shared_ptr<hittable> p, float angle) : ptr(p){
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0,1,bbox);

    point3 min(infinity,infinity,infinity);
    point3 max(-infinity,-infinity,-infinity);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                auto x = i*bbox.max().x + (1-i)*bbox.min().x;
                auto y = j*bbox.max().y + (1-j)*bbox.min().y;
                auto z = k*bbox.max().z + (1-k)*bbox.min().z;

                auto newx = cos_theta*x + sin_theta*z;
                auto newz = -sin_theta*x + cos_theta*z;

                Vec3f tester(newx,y,newz);

                for(int c=0; c<3; c++){
                    min[c] = fmin(min[c],tester[c]);
                    max[c] = fmax(max[c],tester[c]);
                }
            }
        }
    }
    
    bbox = aabb(min,max);

}

bool rotate_y::hit(const ray& r, float t_min, float t_max, hit_record& rec) const{

    auto origin = r.orig;
    auto direction = r.dir;

    //key1: ray to orign position
    origin[0] = cos_theta*r.orig[0] - sin_theta*r.orig[2];
    origin[2] = sin_theta*r.orig[0] + cos_theta*r.orig[2];

    direction[0] = cos_theta*r.dir[0] - sin_theta*r.dir[2];
    direction[2] = sin_theta*r.dir[0] + cos_theta*r.dir[2];

    ray rotate_r(origin, direction, r.time());

    if(!ptr->hit(rotate_r, t_min, t_max, rec))
        return false;
    
    auto p = rec.p;
    auto normal = rec.normal;

    //key2: recover to rotated position

    p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
    p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

    normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
    normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

    rec.p = p;
    rec.set_face_normal(rotate_r, normal);//?

    return true;
}

class flip_face : public hittable{
    public:
        flip_face(shared_ptr<hittable> p) : ptr(p){}

        virtual bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const override{
            if(!ptr->hit(r,t_min,t_max,rec))
                return false;
            rec.front_face = !rec.front_face;
            return true;
        }

        virtual bool bounding_box(float time0, float time1, aabb& output_box) const override{
            return ptr->bounding_box(time0, time1, output_box);
        }

    public:
        shared_ptr<hittable> ptr;
};

#endif
