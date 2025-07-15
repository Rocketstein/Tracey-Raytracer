#include "aabb.h"
AABB::AABB() {
    auto lo_val = std::numeric_limits<double>::infinity();
    auto hi_val = -std::numeric_limits<double>::infinity();

    lo = vec3(lo_val, lo_val, lo_val);
    hi = vec3(hi_val, hi_val, hi_val);
}

AABB::AABB(const vec3& lo, const vec3& hi) : lo(lo - EPSILON), hi(hi + EPSILON) {}

AABB::AABB(const AABB& ab0, const AABB& ab1) {
    auto lo0 = ab0.get_lo();
    auto hi0 = ab0.get_hi();
    auto lo1 = ab1.get_lo();
    auto hi1 = ab1.get_hi();

    double lx, ly, lz, hx, hy, hz;

    lx = lo0.x() < lo1.x() ? lo0.x() : lo1.x();
    ly = lo0.y() < lo1.y() ? lo0.y() : lo1.y();
    lz = lo0.z() < lo1.z() ? lo0.z() : lo1.z();
    hx = hi0.x() > hi1.x() ? hi0.x() : hi1.x();
    hy = hi0.y() > hi1.y() ? hi0.y() : hi1.y();
    hz = hi0.z() > hi1.z() ? hi0.z() : hi1.z();

    lo = vec3(lx, ly, lz);
    hi = vec3(hx, hy, hz);
}

vec3 AABB::get_lo() const {
    return lo;
}

vec3 AABB::get_hi() const {
    return hi;
}

bool AABB::ray_hit(const ray& r, double t_lo, double t_hi) {
    // Check ray-box intersection using the slab method for AABB
    auto ray_dir = r.get_direction();
    auto x_dir   = ray_dir.x();
    auto y_dir   = ray_dir.y();
    auto z_dir   = ray_dir.z();

    auto lo_shift = lo - r.get_origin();
    auto hi_shift = hi - r.get_origin();

    auto t_min = t_lo;
    auto t_max = t_hi;

    // Check x-intersection
    if (std::abs(ray_dir.x()) < 1e-8) {
        if (r.get_origin().x() < lo.x() || r.get_origin().x() > hi.x()) {
            return false;
        }
    }
    auto x0 = lo_shift.x() / x_dir;
    auto x1 = hi_shift.x() / x_dir;
    t_min = std::max(t_min, std::min(x0, x1));
    t_max = std::min(t_max, std::max(x0, x1));
    if (t_min >= t_max) {
        return false;
    }

    // Check y-intersection
    if (std::abs(ray_dir.y()) < 1e-8) {
        if (r.get_origin().y() < lo.y() || r.get_origin().y() > hi.y()) {
            return false;
        }
    }
    auto y0 = lo_shift.y() / y_dir;
    auto y1 = hi_shift.y() / y_dir;
    t_min = std::max(t_min, std::min(y0, y1));
    t_max = std::min(t_max, std::max(y0, y1));
    if (t_min >= t_max) {
        return false;
    }

    // Check z-intersection
    if (std::abs(ray_dir.z()) < 1e-8) {
        if (r.get_origin().z() < lo.z() || r.get_origin().z() > hi.z()) {
            return false;
        }
    }
    auto z0 = lo_shift.z() / z_dir;
    auto z1 = hi_shift.z() / z_dir;
    t_min = std::max(t_min, std::min(z0, z1));
    t_max = std::min(t_max, std::max(z0, z1));
    if (t_min >= t_max) {
        return false;
    }

    return true;
}

