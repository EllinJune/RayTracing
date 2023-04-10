#ifndef LIGHTS_DIFFUSE_H
#define LIGHTS_DIFFUSE_H

#include"rtweekend.h"
#include"material.h"

class diffuse_light : public material

{
public:
    diffuse_light(shared_ptr<texture> a) : emit(a){}
    diffuse_light(color c) : emit(make_shared<solid_color>(c)){}

    virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
        return false;
    }

    virtual color emitted(const ray& r_in, const hit_record& rec, float u, float v, const point3& p) const override{

        if(rec.front_face)
            return emit->value(u,v,p);
        else 
            return color(0,0,0);
    }

public:
    shared_ptr<texture> emit;

};



#endif