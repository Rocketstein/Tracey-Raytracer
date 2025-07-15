#include "objs.h"

AABB objs::translate_aabb(const AABB& aabb, const vec3& offset) {
    return AABB(aabb.get_lo() + offset, aabb.get_hi() + offset);
}

vec3 objs::rotate_vector(const vec3& xyz, double theta, char axis) {
    auto cos_theta = cos(utils::deg_to_rad(theta));
    auto sin_theta = sin(utils::deg_to_rad(theta));
    auto x = xyz.x();
    auto y = xyz.y();
    auto z = xyz.z();

    double new_x, new_y, new_z;

    if (axis == 'x') {
        new_x = x;
        new_y = cos_theta * y - sin_theta * z;
        new_z = sin_theta * y + cos_theta * z;
    } else if (axis == 'y') {
        new_x = cos_theta * x + sin_theta * z;
        new_y = y;
        new_z = -sin_theta * x + cos_theta * z;
    } else if (axis == 'z') {
        new_x = cos_theta * x - sin_theta * y;
        new_y = sin_theta * x + cos_theta * y;
        new_z = z;
    } else {
        std::cerr << "Rotation: Invalid Axis" << std::endl;
        exit(1);
    }

    return vec3(new_x, new_y, new_z);
}