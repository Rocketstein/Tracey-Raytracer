#include "world.h"

world::world() {}
world::world(shared_ptr<objs> object) {
    objects.push_back(object);
    aabb = object->bounding_volume();
}

AABB world::bounding_volume() const {
    return aabb;
}

void world::insert(shared_ptr<objs> object) {
    objects.push_back(object);
    aabb = AABB(aabb, object->bounding_volume());
}

void world::wipe() {
    objects.clear();
    aabb = AABB();
}

void world::translate(const vec3& offset) {
    for (auto object : objects) {
        object->translate(offset);
    }
}

void world::rotate(double theta, char axis) {
    for (auto object : objects) {
        object->rotate(theta, axis);
    }
}

// Maybe return the closest object hit (null for none) instead of bool? 
bool world::ray_hit(const ray& r, double t_lo, double t_hi, hit_history &hist) {
    hit_history tmp;
    bool hit = false;
    auto nearest = t_hi;

    for (const auto &object : objects) {
        if (object->ray_hit(r, t_lo, nearest, tmp)) {
            hit = true;
            nearest = tmp.t;
            hist = tmp;
        }
    }

    return hit;
}