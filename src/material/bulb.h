#ifndef BULB_H
#define BULB_H

#include "material.h"

class Bulb : public material {
    public:
        Bulb(const vec3& alb);
        vec3 emit(const vec3& point) const override;
        tuple<vec3, ray> scatter(const ray &r, const vec3& normal, const vec3& intersection, bool front, double u, double v) const override;
};

#endif