#ifndef METAL_H
#define METAL_H

#include "material.h"

class metal : public material {
    private:
        // Note: ranges from 0 to 1. 
        double fuzziness = 0;
    public:
        metal(const vec3& alb);
        metal(const vec3& alb, double fuzz);
        tuple<vec3, ray> scatter(const ray &r, const vec3& normal, const vec3& intersection, bool front, double u, double v) const override;
};

#endif