#ifndef TEXTURES_IMAGE_H
#define TEXTURES_IMAGE_H

#include"rtweekend.h"
#include"rtw_stb_image.h"
#include"texture.h"

#include<iostream>

class image_texture : public texture
{
public:
    const static int bytes_per_pixel = 3;

    image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}
    image_texture(const char* filename){
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(filename,&width,&height,&components_per_pixel,components_per_pixel);

        if(!data){
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }
    ~image_texture(){
        delete data;
    }

    virtual color value(float u, float v, const point3& p) const override{
        // if have no texture data, return solid cyan as a debugging aid
        if(data == nullptr){
            return color(0,1,1);
        }

        //clamp input texture coordinate to [0,1] x [1,0]
        u = clamp(u,0.,1.);
        v = 1. - clamp(v,0.,1.);

        auto i = static_cast<int>(u*width);
        auto j = static_cast<int>(v*height);

        //clamp integer mapping, since actual coordinates should be less than 1.0;
        if(i >= width) i = width - 1;
        if(j >= height) j = height - 1;

        const auto color_scale = 1.0 / 255.0;
        auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;

        return color(pixel[0],pixel[1],pixel[2]) * color_scale;
    }

private:
    unsigned char *data;
    int width,height;
    int bytes_per_scanline;

};



#endif