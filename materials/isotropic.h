#ifndef MATERIALS_ISOTROPIC_H
#define MATERIALS_ISOTROPIC_H

#include"rtweekend.h"
#include"material.h"

//各向同性
class IsotropicMaterial : public material
{
public:
    IsotropicMaterial(color c) : albedo(make_shared<solid_color>(c)) {}
    IsotropicMaterial(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override{
        // picks a uniform random direction

        srec.specular_ray = ray(rec.p, random_in_unit_sphere(), r_in.time());
        srec.attenuation = albedo->value(rec.u,rec.v,rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;


};





#endif