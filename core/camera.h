#ifndef CAMERA_H
#define CAMERA_H

#include"rtweekend.h"

class camera
{
public:
    camera(){}
    camera(
        point3 lookfrom,
        point3 lookat,
        Vec3f vup,
        float vfov, // vertivcle fov in degrees
        float aspect_ratio,
        float aperture, //光圈
        float focus_dist,//焦平面
        float _time0 = 0,
        float _time1 = 0
    ){
        //视平面 z = -1;成像平面
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);//a ratio to that focus_lenght, may 0.5*height / z = h;
        auto viewport_height = h*2.f;
        auto viewport_width = viewport_height*aspect_ratio;

        w = (lookfrom - lookat).normalize();
        u = cross(vup, w).normalize();
        v = cross(w,u);

        origin = lookfrom;
        //焦平面的宽高，及焦平面的左下角
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;

        lens_radius = aperture / 2;
        time0 = _time0;
        time1 = _time1;
    }

    ray get_ray(float s, float t) const {
        Vec3f rd = lens_radius * random_in_unit_disk();
        Vec3f offset = u*rd.x + v*rd.y;

        return ray(
            origin + offset, 
            lower_left_corner + s*horizontal + t*vertical - origin - offset,
            random_float(time0,time1)
        );
    }

private:
    point3 origin;
    point3 lower_left_corner;
    Vec3f horizontal;
    Vec3f vertical;
    Vec3f u,v,w;
    float lens_radius;
    float time0,time1; // shutter open/close time
};



#endif