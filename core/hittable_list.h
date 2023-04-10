#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include"hittable.h"
#include<memory>
#include<vector>
#include"aabb.h"

using std::shared_ptr;
using std::make_shared;


class hittable_list : public hittable
{
public:
    hittable_list(){}
    hittable_list(shared_ptr<hittable> object){ add(object);}

    void clear(){ objects.clear();};
    void add(shared_ptr<hittable> object){ objects.emplace_back(object);}

    virtual bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const override;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;
    virtual float pdf_value(const point3& origin, const Vec3f& v) const override;
    virtual Vec3f random(const Vec3f& origin) const override;
    

public:
    std::vector<shared_ptr<hittable>> objects;

};

bool hittable_list::hit(const ray &r, float t_min, float t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for(const auto& object:objects){
        if(object->hit(r, t_min, closest_so_far, temp_rec)){
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

bool hittable_list::bounding_box(float time0, float time1, aabb& output_box) const {
    if(objects.empty()) return false;

    //bvh construction
    aabb temp_box;
    bool first_box = true;

    for(const auto& object:objects){
        if(!object->bounding_box(time0, time1, temp_box)) return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }


    return true;
}

float hittable_list::pdf_value(const point3& origin, const Vec3f& v) const {
    auto weight = 1.0/objects.size();
    auto sum = 0.0;

    for(const auto& object:objects){
        sum+=weight*object->pdf_value(origin,v);
    }
    return sum;
}
Vec3f hittable_list::random(const Vec3f& origin) const {
    auto int_size = static_cast<int>(objects.size());
    
    return objects[random_int(0,int_size-1)]->random(origin);
    // try
    // {   
    //     if(temp!=0) throw 1;
    //     return objects[temp]->random(origin);
    // }
    // catch(int)
    // {
    //     std::cerr <<"temp:"<< temp << '\n';
    // }
    
}

#endif