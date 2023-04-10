#ifndef MATERIALS_LAMBERTIAN_H
#define MATERIALS_LAMBERTIAN_H

#include"material.h"
#include"texture.h"
#include"onb.h"

class LambertianMaterial : public material
{
public:
    shared_ptr<texture> albedo;


public:
    LambertianMaterial(const color& a): albedo(make_shared<solid_color>(a)){}
    LambertianMaterial(shared_ptr<texture> a): albedo(a){}

    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override{
        // auto scatter_direction = rec.normal + random_unit_vector();
        
        // version 1.0
        //  // Catch degenerate scatter direction
        // if (scatter_direction.near_zero())
        //     scatter_direction = rec.normal;

        // scattered = ray(rec.p, scatter_direction.normalize(), r_in.time());
        // alb = albedo->value(rec.u,rec.v,rec.p);
        // pdf = rec.normal*scattered.dir / pi; // cos_theta / pi;
        // // attenuation = albedo->value(rec.u,rec.v,rec.p); // uv is useful, but rec.p isn't in this case

        //version 2.0
        // onb uvw;
        // uvw.build_from_w(rec.normal);
        // auto direction = uvw.local(random_cosine_direction());
        // scattered = ray(rec.p, direction.normalize(), r_in.time());
        // alb = albedo->value(rec.u,rec.v,rec.p);
        // // pdf = 0.5 / pi;
        // pdf = uvw.w()*scattered.dir / pi;

        //version 3.0
        srec.is_specular = false;
        srec.attenuation = albedo->value(rec.u,rec.v,rec.p);
        srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);

        return true;
    }
    float scattering_pdf(const ray& r_in, const hit_record& rec,const ray &scattered) const {
        auto cosine = rec.normal*scattered.direction().normalize();
        return cosine < 0 ? 0 : cosine / pi;
    }
};




#endif