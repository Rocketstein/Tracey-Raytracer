#ifndef MATERIAL_H
#define MATERIAL_H

#include <tuple>
#include "../ray.h"

using std::tuple, std::make_tuple;

class material{
    protected:
        vec3 albedo;
        bool emissive = false;

    public:
        material() = default;
        material(const vec3& albedo_);
        virtual ~material() = default;

        // Returns the illumination created from the designated point
        virtual vec3 emit(const vec3& point) const;

        // tuple<attenuation, resulting secondary ray>
        virtual tuple<vec3, ray> scatter(const ray &r, const vec3& normal, const vec3& intersection, bool front, double u, double v) const = 0;

        // Check if the material is emissive
        bool is_emissive() const;
};

#endif