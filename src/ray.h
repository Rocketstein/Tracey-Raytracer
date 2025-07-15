#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
    private:
        vec3 origin;
        vec3 direction;

    public:
        ray(const vec3 &init, const vec3 &dir);

        vec3 get_origin() const;
        vec3 get_direction() const;
        vec3 parametric_loc(double t) const;
};

#endif