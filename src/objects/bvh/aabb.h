#ifndef AABB_H
#define AABB_H

// #include "../../ray.h"
// #include "../world.h"
// #include "../objs.h"

#include "../../vec3.h"
#include "../../ray.h"
#include <memory>

using std::make_shared, std::shared_ptr;

class AABB {
    public:
        AABB();
        AABB(const vec3& lo, const vec3& hi);
        AABB(const AABB& ab0, const AABB& ab1);
        // AABB bounding_volume() const override;

        vec3 get_lo() const;
        vec3 get_hi() const;

        bool ray_hit(const ray& r, double t_lo, double t_hi);

    private:
        // Bounding range
        vec3 lo;
        vec3 hi;
};

#endif