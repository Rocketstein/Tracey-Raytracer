#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include "objs.h"

using std::make_shared;
using std::shared_ptr;

class world : public objs {
    public:
        std::vector<shared_ptr<objs>> objects;
        world();
        world(shared_ptr<objs> object);

        void insert(shared_ptr<objs> object);
        void wipe();

        bool ray_hit(const ray& r, double t_lo, double t_hi, hit_history &hist) override;

        AABB bounding_volume() const override;

        void translate(const vec3& offset) override;

        void rotate(double theta, char axis) override;
    
        private:
            AABB aabb;
};

#endif