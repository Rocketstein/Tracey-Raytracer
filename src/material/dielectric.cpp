#include "dielectric.h"

dielectric::dielectric(double refract_index) : ior(refract_index) {}

tuple<vec3, ray> dielectric::scatter(const ray &r, const vec3& normal, const vec3& intersection, bool front, double u, double v) const {
    auto unit_normal = normal.unit_vector();
    auto d = r.get_direction().unit_vector();
    double refraction = ior;
    if (front) {
        refraction = 1 / refraction;
    } 

    auto k = 1.0 - pow(refraction, 2) * (1.0 - pow((unit_normal * d), 2));
    double cos_theta = fmin((-1 *d) * normal, 1.0);
    
    vec3 secondary_dir;
    if (k < 0 || schlick(refraction, unit_normal, d) > utils::random_double(0, 1)) {
        secondary_dir = d - unit_normal * (unit_normal * d) * 2;
    } else {
        secondary_dir = d * refraction - unit_normal * (refraction * (unit_normal * d) + sqrt(k));
    }

    return make_tuple(vec3(1.0, 1.0, 1.0), ray(intersection, secondary_dir));
}

double dielectric::schlick(double ref, vec3 normal, vec3 vec) const {
    auto cos = (-1 * vec) * normal;
    if (cos > 1.0) {
        cos = 1.0;
    }

    auto f0 = pow(ref - 1, 2) / pow(ref + 1, 2);
    return f0 + (1 - f0) * pow((1 - cos), 5);
}