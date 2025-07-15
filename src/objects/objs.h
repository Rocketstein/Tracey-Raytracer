#ifndef OBJS_H
#define OBJS_H

#include <memory>
#include "../material/material.h"
#include "bvh/aabb.h"

using std::shared_ptr;

struct hit_history {
    double  t1;
    double  t2;
    double  t;
    double  u;
    double  v;
    vec3    intersection;
    vec3    normal;
    bool    is_front;
    shared_ptr<material> material_;
};

// Abstract parent class for surface objects (sphere, triangle, plane, etc.)
class objs {
    public:
        virtual ~objs() = default;
        virtual bool ray_hit(const ray& r, double t_lo, double t_hi, hit_history &hist) = 0;
        virtual AABB bounding_volume() const = 0;
        virtual void translate(const vec3& offset) = 0;
        virtual void rotate(double theta, char axis) = 0;
        
    protected:
        AABB translate_aabb(const AABB& aabb, const vec3& offset);
        vec3 rotate_vector(const vec3& xyz, double theta, char axis);
};

#endif