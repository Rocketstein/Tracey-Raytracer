#include "material.h"

material::material(const vec3& albedo_) : albedo(albedo_) {}

vec3 material::emit(const vec3& point) const {
    // Zero-light emission by default
    return vec3(0,0,0);
}

bool material::is_emissive() const {
    return emissive;
}