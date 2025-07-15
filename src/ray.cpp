#include "ray.h"

ray::ray(const vec3 &init, const vec3 &dir) : origin(init), direction(dir.unit_vector()) {}

vec3 ray::get_origin() const {
    return origin;
}

vec3 ray::get_direction() const {
    return direction.unit_vector();
}

vec3 ray::parametric_loc(double t) const {
    return origin + direction * t;
}