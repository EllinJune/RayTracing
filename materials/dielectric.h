#ifndef MATERIALS_DIELECTRIC_H
#define MATERIALS_DIELECTRIC_H

#include"material.h"

class DielectricMaterial : public material {
    public:
        float refract_index;
    public:
        DielectricMaterial(float ir) : refract_index(ir) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const override {
            srec.is_specular = true;
            srec.pdf_ptr = nullptr;
            srec.attenuation = color(1.0,1.0,1.0);//衰减始终为1，因为玻璃表面不吸收任何东西。
            //判断交点是在外部还是内部 front_face为true时-->外部
            float refraction_ratio = rec.front_face ? (1.f/refract_index) : refract_index;

            Vec3f unit_direction = Vec3f(r_in.dir).normalize();
            float cos_theta = fmin(-unit_direction*rec.normal, 1.0);
            float sin_theta = sqrtf(1-cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio*sin_theta > 1.;
            Vec3f direction;

            if(cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);
            
            srec.specular_ray = ray(rec.p, direction, r_in.time());
            return true;
        }
    private:
        static float reflectance(float cosine, float ref_idx){//反射率,玻璃的反射率是随坡度变化的
            // Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 *=r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }
};

#endif