#ifndef BVH_H
#define BVH_H

#include "../world.h"
#include "aabb.h"
#include <algorithm>

using std::vector, std::shared_ptr;

class BoundingVolumeNode : public objs {
    public:
        BoundingVolumeNode(world& w);
        BoundingVolumeNode(vector<shared_ptr<objs>>& objects, int lo, int hi);

        bool ray_hit(const ray& r, double t_lo, double t_hi, hit_history &hist) override;
        AABB bounding_volume() const override;
        void translate(const vec3& offset) override;
        void rotate(double theta, char axis) override;

    private:
        shared_ptr<objs> lchild = nullptr;
        shared_ptr<objs> rchild = nullptr;
        AABB aabb;
};

#endif