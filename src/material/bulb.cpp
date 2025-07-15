#include "bulb.h"

Bulb::Bulb(const vec3& alb) : material(alb) {
    emissive = true;
}

vec3 Bulb::emit(const vec3& point) const {
    // Not really an albedo, but for simplicity's sake
    return albedo;
}

tuple<vec3, ray> Bulb::scatter(const ray &r, const vec3& normal, const vec3& intersection, bool front, double u, double v) const {
    auto secondary_dir = vec3(-1, -1, -1);                      // Sentinel value
    auto secondary_ray = ray(intersection, secondary_dir);

    return make_tuple(albedo, secondary_ray);
}