#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "material.h"

class dielectric : public material {
    private:
        double ior;

        // Performs a Schlick approximation using unit normal and incident vectors.
        double schlick(double ref, vec3 normal, vec3 vec) const;

    public:
        dielectric(double refract_index);

        // Calculates the refracted ray using Snell's law. May return total internal reflection.
        tuple<vec3, ray> scatter(const ray &r, const vec3& normal, const vec3& intersection, bool front, double u, double v) const override;
};

#endif