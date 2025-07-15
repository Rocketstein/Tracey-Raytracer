#include "metal.h"

metal::metal(const vec3& alb) : material(alb) {}

metal::metal(const vec3& alb, double fuzz) : material(alb), fuzziness(fuzz) {}

tuple<vec3, ray> metal::scatter(const ray &r, const vec3& normal, const vec3& intersection, bool front, double u, double v) const {
    // https://inhopp.github.io/graphics/graphics9/ -- Reflection formula
    auto d = r.get_direction();
    auto secondary_dir = d - normal * (normal * d) * 2
    + (random_vector(1)) * fuzziness;
    auto secondary_ray = ray(intersection, secondary_dir);
    return make_tuple(albedo, secondary_ray);
}