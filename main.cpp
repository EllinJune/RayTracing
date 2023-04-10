#include"rtweekend.h"
#include"camera.h"
#include"color.h"
#include"hittable_list.h"
#include"sphere.h"
#include"lambertian.h"
#include"metal.h"
#include"dielectric.h"
#include"moving_sphere.h"
#include"checker.h"
#include"perlin.h"
#include"image.h"
#include"diffuse.h"
#include"aarect.h"
#include"box.h"
#include"constant_medium.h"
#include"isotropic.h"
#include"bvh.h"
#include"pdf.h"

#include<iostream>
#include"rtw_stb_image.h"

//image
float aspect_ratio = 16.f / 9.f;
int image_width = 400;
int image_height = static_cast<int>(image_width/aspect_ratio);
int samples_per_pixel = 100;
const int max_depth = 50;

//camera
point3 lookfrom(13,2,3);
point3 lookat(0,0,0);
Vec3f vup(0,1,0);
// auto dist_to_focus = (lookfrom-lookat).norm();
auto dist_to_focus = 10.0;
auto vfov = 40.0;
auto aperture = 0.0;
// auto aperture = 2.0; //光圈
color background(0,0,0);

auto time0=0.0;
auto time1=1.0;

camera cam;

//World objects
hittable_list world;
//lights;
// hittable_list lights;
auto lights = make_shared<hittable_list>();

/////////////////////////////////////////////////////////////////////////////////////////////////////////
hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    //? how to construct
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<LambertianMaterial>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_float();
            point3 center(a + 0.9*random_float(), 0.2, b + 0.9*random_float());

            if ((center - point3(4, 0.2, 0)).norm() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = attenuation_ray_color(random_v3f(), random_v3f());
                    sphere_material = make_shared<LambertianMaterial>(albedo);
                    auto center2 = center + Vec3f(0, random_float(0,.5),0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random_v3f(0.5, 1);
                    auto fuzz = random_float(0, 0.5);
                    sphere_material = make_shared<MetalMaterial>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<DielectricMaterial>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<DielectricMaterial>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<LambertianMaterial>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<MetalMaterial>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
hittable_list two_spheres(){
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    // auto pertext = make_shared<noise_texture>();

    objects.add(make_shared<sphere>(point3(0,-10,0), 10, make_shared<LambertianMaterial>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10,0), 10, make_shared<LambertianMaterial>(checker)));

    return objects;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
hittable_list two_perlin_spheres(){
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);

    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<LambertianMaterial>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2,0), 2, make_shared<LambertianMaterial>(pertext)));

    return objects;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
hittable_list earth(){
    auto earth_texture = make_shared<image_texture>("../earthmap.jpg");
    auto earth_surface = make_shared<LambertianMaterial>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2,earth_surface);

    return hittable_list(globe);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
hittable_list simple_light(){
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<LambertianMaterial>(pertext)));
    objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<LambertianMaterial>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));//This allows it to be bright enough to light things.
    objects.add(make_shared<xy_rect>(3,5,1,3,-2,difflight));

    return objects;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
hittable_list cornell_box(){
    hittable_list objects;

    auto red = make_shared<LambertianMaterial>(color(.65,.05,.05));
    auto white = make_shared<LambertianMaterial>(color(.73,.73,.73));
    auto green = make_shared<LambertianMaterial>(color(.12,.45,.15));
    auto light = make_shared<diffuse_light>(color(15,15,15));

    objects.add(make_shared<yz_rect>(0,555,0,555,555,green));
    objects.add(make_shared<yz_rect>(0,555,0,555,0,red));
    // objects.add(make_shared<xz_rect>(213,343,227,332,554,light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213,343,227,332,554,light)));
    objects.add(make_shared<xz_rect>(0,555,0,555,0,white));
    objects.add(make_shared<xz_rect>(0,555,0,555,555,white));
    objects.add(make_shared<xy_rect>(0,555,0,555,555,white));


    //左
    shared_ptr<material> aluminum = make_shared<MetalMaterial>(color(0.8,0.85,0.88), 0.0);
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1,15);
    box1 = make_shared<translate>(box1, Vec3f(265,0,295));// 因为观察的视角从z:-800到0，因此左右和前后看上去是颠倒的
    objects.add(box1);

    //右
    shared_ptr<material> glass = make_shared<DielectricMaterial>(1.5);
    // shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    // box2 = make_shared<rotate_y>(box2, -18);
    // box2 = make_shared<translate>(box2, Vec3f(130,0,65));
    // objects.add(box2);
    objects.add(make_shared<sphere>(point3(190,90,190),90,glass));

    return objects;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
hittable_list cornell_smoke(){
    hittable_list objects;

    auto red = make_shared<LambertianMaterial>(color(.65,.05,.05));
    auto white = make_shared<LambertianMaterial>(color(.73,.73,.73));
    auto green = make_shared<LambertianMaterial>(color(.12,.45,.15));
    auto light = make_shared<diffuse_light>(color(7,7,7));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    //右
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1,15);
    box1 = make_shared<translate>(box1, Vec3f(265,0,295));// 因为观察的视角从z:-800到0，因此左右和前后看上去是颠倒的

    //左
    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, Vec3f(130,0,65));

    objects.add(make_shared<constant_medium>(box1, 0.01, color(0,0,0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, color(1,1,1)));

    return objects;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
hittable_list final_scene(){
    //ground
    hittable_list boxes1;
    auto ground = make_shared<LambertianMaterial>(color(0.48,0.83,0.53));

    const int boxes_per_side = 20;
    for(int i=0; i<boxes_per_side; i++){
        for(int j=0;j<boxes_per_side;j++){
            auto w = 100.f;
            auto x0 = -1000.f + i*w;
            auto z0 = -1000.f + j*w;
            auto y0 = 0.f;
            auto x1 = x0 + w;
            auto y1 = random_float(1,101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }

    hittable_list objects;
    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    //light
    auto light = make_shared<diffuse_light>(color(7,7,7));
    objects.add(make_shared<xz_rect>(123,423,147,412,554,light));

    //moving sphere
    auto center1 = point3(400,400,200);
    auto center2 = center1 + Vec3f(30,0,0);
    auto moving_sphere_material = make_shared<LambertianMaterial>(color(0.7,0.3,0.1));
    objects.add(make_shared<moving_sphere>(center1,center2,0,1,50,moving_sphere_material));
    // refract with shilick and reflect with fuzz 
    objects.add(make_shared<sphere>(point3(260,150,45), 50, make_shared<DielectricMaterial>(1.5)));
    objects.add(make_shared<sphere>(point3(0,150,145), 50, make_shared<MetalMaterial>(color(.8,.8,.9), 1.0)));
    // volumes: constant_density_medium
    auto boundary = make_shared<sphere>(point3(360,150,145),70,make_shared<DielectricMaterial>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(.2,.4,.9)));
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<DielectricMaterial>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));
    // image map , perlin noise
    auto emat = make_shared<LambertianMaterial>(make_shared<image_texture>("../earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220,280,300),80, make_shared<LambertianMaterial>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<LambertianMaterial>(color(.73,.73,.73));
    int ns =1000;
    for(int j=0; j<ns; j++){
        boxes2.add(make_shared<sphere>(random_v3f(0, 165), 10, white));
    }
    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0,1.0), 15),
            Vec3f(-100,270,395)
        )
    );
    
    return objects;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
Vec3f getLight(const ray& r, const hittable&world){
    hit_record rec;
    scatter_record srec;
    if(world.hit(r,1e-3,infinity, rec)){
        color emitted = rec.mat_ptr->emitted(r,rec,rec.u,rec.v,rec.p);
        return emitted;
    }
    else return Vec3f(0,0,0);

}


color ray_color(
    const ray& r, 
    const color& background, 
    const hittable& world, 
    shared_ptr<hittable> lights, 
    int depth
){
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if(depth<=0) return color(0,0,0);


    hit_record rec;

    //if the ray hits nothing ,return the background color
    //t_min ： to ignore hits very near zero
    if(!world.hit(r,1e-3,infinity,rec))
        return background;


    scatter_record srec;
    color emitted = rec.mat_ptr->emitted(r,rec,rec.u,rec.v,rec.p);
    if(!rec.mat_ptr->scatter(r,rec,srec))
        return emitted;


    if(srec.is_specular){
        return attenuation_ray_color(srec.attenuation, ray_color(srec.specular_ray, background, world, lights, depth-1));
    }

    
    auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
    mixture_pdf p(light_ptr, srec.pdf_ptr); // pdf

    ray scattered = ray(rec.p, p.generate(), r.time());

    auto pdf_val = p.value(scattered.dir);

    return emitted + attenuation_ray_color(
        srec.attenuation * rec.mat_ptr->scattering_pdf(r,rec,scattered), 
        ray_color(scattered, background, world, lights, depth-1)) / pdf_val;
}

color WhittedRT(
    const ray& r, 
    const color& background, 
    const hittable& world, 
    shared_ptr<hittable> lights, 
    int depth
){
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if(depth<=0) return color(0,0,0);


    hit_record rec;
    scatter_record srec;
    color finalColor(0,0,0);
    //没有击中物体
    if (!world.hit(r,1e-3,infinity,rec))
        return background;
    
    Vec3f emitted=rec.mat_ptr->emitted(r,rec,rec.u,rec.v,rec.p);

    //击中了物体
    if(rec.mat_ptr->scatter(r,rec,srec)){
        if (srec.is_specular)
        {
            finalColor = attenuation_ray_color(srec.attenuation, WhittedRT(srec.specular_ray, background, world, lights, depth-1));
        }
        else{ //计算对光采样的pdf
            auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
            ray scattered = ray(rec.p, light_ptr->generate(), r.time());
            //对光重要性采样的pdf
            float pdf_val = light_ptr->value(scattered.direction());
            //计算散射pdf
            float mpdf = rec.mat_ptr->scattering_pdf(r,rec,scattered);
            //计算光照强度
            // Vec3f Li = getLight(scattered,world);
            // finalColor += attenuation_ray_color(srec.attenuation*mpdf, Li) /pdf_val;
            finalColor = attenuation_ray_color(srec.attenuation*mpdf, WhittedRT(scattered, background, world, lights, depth-1))/pdf_val;
        
        }
        
    }
    else{ //击中了光源
        finalColor = emitted;
    }
    return finalColor;
}

void render(){
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    std::vector<Vec3f> framebuffer(image_height*image_width);
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            for(int s=0;s<samples_per_pixel;s++){
                auto u = (i+random_float()) / (image_width-1);
                auto v = 1.0 - (j+random_float()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                
                pixel_color+=WhittedRT(r, background, world, lights, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel, framebuffer[i+j*image_width]);
        }
    }

    std::vector<unsigned char> pixmap(image_width*image_height*3);
    for (size_t i = 0; i < image_width*image_height; ++i) {
        Vec3f &c = framebuffer[i];
        float max = std::max(c[0], std::max(c[1], c[2]));
        if (max>1) c = c*(1./max);
        for (size_t j = 0; j<3; j++) {
            pixmap[i*3+j] = (unsigned char)(256 *  framebuffer[i][j]);
        }
    }
    stbi_write_jpg("../out.jpg", image_width, image_height, 3, pixmap.data(), 100);

    std::cerr << "\nDone.\n";


}

int main(){
    switch (6)
    {
    case 1:
        world = random_scene();
        background = color(0.7f, 0.8f, 1.f );
        lookfrom = point3(13,2,3);
        lookat = point3(0,0,0);
        vfov = 20.0;
        aperture = 0.1;
        break;
    case 2:
        world = two_spheres();
        background = color(0.7f, 0.8f, 1.f );
        lookfrom = point3(13,2,3);
        lookat = point3(0,0,0);
        vfov = 20.0;
        break;
    
    case 3:
        world = two_perlin_spheres() ;
        background = color(0.7f, 0.8f, 1.f );
        lookfrom = point3(13,2,3);
        lookat = point3(0,0,0);
        vfov = 20.0;
        break;
    case 4:
        world = earth() ;
        background = color(0.7f, 0.8f, 1.f );
        lookfrom = point3(13,2,3);
        lookat = point3(0,0,0);
        vfov = 20.0;
        break;

    case 5:
        world = simple_light();
        samples_per_pixel = 400;
        background = color(0,0,0);
        lookfrom = point3(26,3,6);
        lookat = point3(0,2,0);
        vfov = 20.0;
        break;
    case 6:
        world = cornell_box();
        // auto lights = make_shared<hittable_list>();
        lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
        // lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));
        aspect_ratio = 1.0;
        image_width = 600;
        image_height = static_cast<int>(image_width/aspect_ratio);
        samples_per_pixel = 100;
        background = color(0,0,0);
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vup = Vec3f(0,1,0);
        vfov = 40.0;
        dist_to_focus = 10.0;
        aperture = 0.0;
        time0=0.0;
        time1=1.0;
        break;
    case 7:
        world = cornell_smoke();
        aspect_ratio = 1.0;
        image_width = 600;
        samples_per_pixel = 200;
        image_height = static_cast<int>(image_width/aspect_ratio);
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    default:
    case 8:
        world = final_scene();
        lights->add(make_shared<xz_rect>(123,423,147,412,554, shared_ptr<material>()));
        aspect_ratio = 1.0;
        image_width = 800;
        image_height = static_cast<int>(image_width/aspect_ratio);
        samples_per_pixel = 1000;
        background = color(0,0,0);
        lookfrom = point3(478, 278, -600);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    }
    
    cam = camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, time0, time1);


    render();

    
    return 0;
}