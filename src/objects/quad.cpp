#include "quad.h"

Quad::Quad(const vec3& q, const vec3& u_, const vec3& v_, shared_ptr<material> mat) 
    : cornerstone(q), u(u_), v(v_), normal(cross(u, v).unit_vector()), material_(mat) {
    
    // Calculate all four corners of the quad
    vec3 p0 = q;           // cornerstone
    vec3 p1 = q + u_;      // cornerstone + u
    vec3 p2 = q + v_;      // cornerstone + v  
    vec3 p3 = q + u_ + v_; // cornerstone + u + v
    
    vec3 min_point = p0;
    vec3 max_point = p0;
    
    auto update_bounds = [&](const vec3& p) {
        min_point = vec3(std::min(min_point.x(), p.x()),
                        std::min(min_point.y(), p.y()),
                        std::min(min_point.z(), p.z()));
        max_point = vec3(std::max(max_point.x(), p.x()),
                        std::max(max_point.y(), p.y()),
                        std::max(max_point.z(), p.z()));
    };
    
    update_bounds(p1);
    update_bounds(p2);
    update_bounds(p3);
    
    aabb = AABB(min_point, max_point);
}

AABB Quad::bounding_volume() const {
    return aabb;
}

bool Quad::ray_hit(const ray& r, double t_lo, double t_hi, hit_history &hist) {
    auto ray_unit_dir = r.get_direction().unit_vector();
    auto denominator = ray_unit_dir * normal;

    if (abs(denominator) < 1e-6) {
        // parallel
        return false;
    }

    auto t = ((cornerstone - r.get_origin()) * normal) / denominator;

    if (t < t_lo || t > t_hi) {
        // Outside the meaningful parametric domain
        return false;
    }

    auto intersection = r.parametric_loc(t);
    auto w = intersection - cornerstone;      // Needed to pass by reference
    auto denom = (u * u) * (v * v) - (u * v) * (u * v);
    auto a = ((w * u) * (v * v) - (w * v) * (u * v)) / denom;
    auto b = ((w * v) * (u * u) - (w * u) * (u * v)) / denom;

    if ((0 <= a && a <= 1) && (0 <= b && b <= 1)) {
        hist.u = a;
        hist.v = b;
    } else {
        // Outside the quadrilateral bounds
        return false;
    }

    hist.t = t;
    hist.intersection = intersection;
    hist.material_ = material_;
    
    if (denominator > 0) {
        hist.is_front = false;
        hist.normal = normal * -1;
    } else {
        hist.is_front = true;
        hist.normal = normal;
    }

    return true;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html

void Quad::translate(const vec3& offset) {
    cornerstone += offset;
    aabb = translate_aabb(aabb, offset);
}

// Caution: Revolves around the cornerstone, not the center
void Quad::rotate(double theta, char axis) {
    vec3 rotated_u = rotate_vector(u, theta, axis);
    vec3 rotated_v = rotate_vector(v, theta, axis);
    *this = Quad(rotate_vector(cornerstone, theta, axis), rotated_u, rotated_v, material_);
}