#ifndef COLOR_H
#define COLOR_H

#include"rtweekend.h"
#include"geometry.h"
#include<iostream>

void write_color(std::ostream &out,color &pixel_color, int samples_per_pixel, Vec3f &pixel){
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;

    //Replace NaN components with zero. 
    try
    {
        if(r!=r || g != g || b != b) throw 1;
    }
    catch(int)
    {
        std::cerr <<"Nan" << '\n';
    }
    
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);


    // Write the translated [0,255] value of each color component.
    // out << static_cast<int>(256 * clamp(r, 0.0, 0.999))<<' '
    //     << static_cast<int>(256* clamp(g, 0.0, 0.999))<<' '
    //     << static_cast<int>(256 * clamp(b, 0.0, 0.999))<<'\n';    
    pixel = Vec3f(clamp(r, 0.0, 0.999),clamp(g, 0.0, 0.999),clamp(b, 0.0, 0.999));
}

#endif