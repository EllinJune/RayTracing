#ifndef MATERIALS_METAL_H
#define MATERIALS_METAL_H

#include"material.h"

class MetalMaterial : public material {
    public:
        color albedo;
        float fuzz;//for fuzzy reflection
    public:
        MetalMaterial(const color& a, float f) : albedo(a), fuzz(f) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            Vec3f reflected = reflect(Vec3f(r_in.dir).normalize(), rec.normal);
            srec.specular_ray = ray(rec.p, reflected + fuzz*random_in_unit_sphere());// add fuzzy reflection
            srec.attenuation = albedo;
            srec.is_specular = true;
            srec.pdf_ptr = 0;
            return true;
        }
};

#endif