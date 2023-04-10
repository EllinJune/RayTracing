#ifndef PDF_H
#define PDF_H

#include"rtweekend.h"
#include"onb.h"

class pdf
{
public:
    virtual ~pdf(){}

    virtual float value(const Vec3f& direction) const = 0;
    virtual Vec3f generate() const = 0;

};

class cosine_pdf : public pdf
{
public:
    cosine_pdf(const Vec3f& w) { uvw.build_from_w(w);}

    virtual float value(const Vec3f& direction) const override{
        auto cosine = Vec3f(direction).normalize() * uvw.w();
        return (cosine <= 0) ? 0: cosine/pi;
    }

    virtual Vec3f generate() const override{
        return uvw.local(random_cosine_direction());
    }

public:
    onb uvw;

};

class hittable_pdf : public pdf
{
public:
    hittable_pdf(shared_ptr<hittable> p, const Vec3f& o): ptr(p), origin(o){}

    virtual float value(const Vec3f& direction) const override{
        return ptr->pdf_value(origin, direction);
    }

    virtual Vec3f generate() const override{
        return ptr->random(origin);
    }

public:
    point3 origin;
    shared_ptr<hittable> ptr;
};

class mixture_pdf : public pdf
{
public:
    mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1){ p[0]=p0; p[1]=p1;}

    virtual float value(const Vec3f& direction) const override{
        return 0.5* p[0]->value(direction) + 0.5*p[1]->value(direction);
    }
    virtual Vec3f generate() const override{
        if(random_float() < 0.5)
            return p[0]->generate();
        else
        {   
            // std::cout<<5<<std::endl;
            return p[1]->generate();}
    }

public:
    shared_ptr<pdf> p[2];
};








#endif