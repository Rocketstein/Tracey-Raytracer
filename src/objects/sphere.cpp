#include "sphere.h"

sphere::sphere(double rad, const vec3& cen, shared_ptr<material> mat) : radius(rad), center(cen), material_(mat) {
    vec3 radvec(rad, rad, rad);
    auto lo = cen - rad;
    auto hi = cen + rad;
    aabb = AABB(lo, hi);
}

bool sphere::ray_hit(const ray& r, double t_lo, double t_hi, hit_history& hist) {
    // // Problematic
    // vec3 o = center - r.get_origin();
    // double tc = (o * r.get_direction());
    // double d2 = (o * o) - tc * tc;

    vec3 o = center - r.get_origin();
    double tc = (o * r.get_direction());
    vec3 closest_point_vec = o - tc * r.get_direction();
    double d2 = closest_point_vec * closest_point_vec;
    double radius2 = radius * radius;

    if (tc < 0 || d2 > radius2) {
        return false;
    }

    double offset = sqrt(radius2 - d2);
    double t1 = tc - offset;
    double t2 = tc + offset;

    double t;
    if (t1 > t_lo && t1 < t_hi) {
        t = t1;
    } else if (t2 > t_lo && t2 < t_hi) {
        t = t2;
    } else {
        return false;
    }

    vec3 intersection = r.parametric_loc(t);
    hist.t = t;
    hist.t1 = t1;
    hist.t2 = t2;
    hist.intersection = intersection;

    vec3 normal = (intersection - center) / radius;
    if ((r.get_direction() * normal.unit_vector()) > 0) {
        hist.is_front = false;
        normal *= -1;
    } else {
        hist.is_front = true;
    }
    hist.normal = normal;
    hist.material_ = material_;

    // Apply rotations in order: X, then Y, then Z
    vec3 rotated_normal = normal;    
    if (x_theta != 0) {
        rotated_normal = rotate_vector(rotated_normal, x_theta, 'x');
    }
    if (y_theta != 0) {
        rotated_normal = rotate_vector(rotated_normal, y_theta, 'y');
    }
    if (z_theta != 0) {
        rotated_normal = rotate_vector(rotated_normal, z_theta, 'z');
    }

    // Calculate UV coordinates using the rotated normal
    hist.u = (std::atan2(-rotated_normal.z(), rotated_normal.x()) + M_PI) / (2 * M_PI);
    hist.v = (std::acos(-rotated_normal.y())) / M_PI;

    // hist.u = (std::atan2(-normal.z(), normal.x()) + M_PI) / (2 * M_PI);
    // hist.v = (std::acos(-normal.y())) / M_PI;

    return true;
}

AABB sphere::bounding_volume() const {
    return aabb;
}

void sphere::translate(const vec3& offset) {
    center += offset;
    aabb = translate_aabb(aabb, offset);
}

void sphere::rotate(double theta, char axis) {
    if (axis == 'x') {
        x_theta += theta;
    } else if (axis == 'y') {
        y_theta += theta;
    } else if (axis == 'z') {
        z_theta += theta;
    } else {
        std::cerr << "Rotation: Invalid Axis" << std::endl;
        exit(1);
    }
}