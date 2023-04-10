#ifndef MATERIAL_H
#define MATERIAL_H

#include"rtweekend.h"
#include"pdf.h"

//使用hit_record是为了避免函数需要过多的参数
struct hit_record;

struct scatter_record{
    ray specular_ray;
    bool is_specular;
    color attenuation;
    shared_ptr<pdf> pdf_ptr;
};

class material
{
public:
    virtual color emitted(const ray& r_in, const hit_record& rec, float u, float v, const point3& p) const{
        return color(0,0,0);
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const {
        return false;
    }
    virtual float scattering_pdf(const ray& r_in, const hit_record& rec,const ray &scattered) const {
        return 0;
    }
    
};


#endif
