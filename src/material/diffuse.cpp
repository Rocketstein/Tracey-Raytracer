#include "diffuse.h"

diffuse::diffuse(const vec3& alb) : material(alb) {}
diffuse::diffuse(shared_ptr<Texture> tex) : texture(tex), use_textures(true) {}

tuple<vec3, ray> diffuse::scatter(const ray &r, const vec3& normal, const vec3& intersection, bool front, double u, double v) const {
    auto secondary_dir = random_vector(1) + normal;
    auto secondary_ray = ray(intersection, secondary_dir);

    if (use_textures) {
        return make_tuple(texture->get_color_at(u, v, intersection), secondary_ray);
    } else {
        return make_tuple(albedo, secondary_ray);
    }

    // return make_tuple(albedo, secondary_ray);
}