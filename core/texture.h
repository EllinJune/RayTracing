#ifndef TEXTURE_H
#define TEXTURE_H

#include"rtweekend.h"

class texture
{
public:
    virtual color value(float u, float v, const point3& p) const = 0;

};


//常量颜色纹理，纹理的颜色就是固定的某种颜色
class solid_color : public texture
{
public:
    solid_color(){}
    solid_color(color c) : color_value(c){}

    solid_color(float red, float green, float blue) : solid_color(color(red,green,blue)){}
    virtual color value(float u, float v, const point3& p) const override{
        return color_value;
    }

private:
    color color_value;


};



#endif