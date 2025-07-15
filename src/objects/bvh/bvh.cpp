#include "bvh.h"

BoundingVolumeNode::BoundingVolumeNode(world& w) : BoundingVolumeNode(w.objects, 0, w.objects.size()) {}

// Helper function to compare objects along a specific axis
bool box_compare(const shared_ptr<objs>& a, const shared_ptr<objs>& b, int axis) {
    auto a_lo = a->bounding_volume().get_lo();
    auto b_lo = b->bounding_volume().get_lo();

    switch (axis) {
        case 0: {
            return a_lo.x() < b_lo.x();
        }
        case 1: {
            return a_lo.y() < b_lo.y();
        }
        default: {
            return a_lo.z() < b_lo.z();
        }
    }
}

BoundingVolumeNode::BoundingVolumeNode(vector<shared_ptr<objs>>& objects, int lo, int hi) {
    int random_axis = static_cast<int>(std::floor(utils::random_double(0, 3)));

    switch (hi - lo) {
        case 1: {
            auto leaf = objects.at(lo);
            lchild = leaf;
            rchild = leaf;
            break;
        }
        case 2: {
            lchild = objects.at(lo);
            rchild = objects.at(lo + 1);
            break;
        }
        default: {
            // Sort objects along the random axis
            std::sort(objects.begin() + lo, objects.begin() + hi, 
                     [random_axis](const shared_ptr<objs>& a, const shared_ptr<objs>& b) {
                         return box_compare(a, b, random_axis);
                     });
            
            auto mid = lo + (hi - lo) / 2;
            lchild = make_shared<BoundingVolumeNode>(objects, lo, mid);
            rchild = make_shared<BoundingVolumeNode>(objects, mid, hi);
        }
    }

    aabb = AABB(lchild->bounding_volume(), rchild->bounding_volume());
}

bool BoundingVolumeNode::ray_hit(const ray& r, double t_lo, double t_hi, hit_history &hist) {
    if (!aabb.ray_hit(r, t_lo, t_hi)) {
        return false;
    }

    bool hit_left = lchild->ray_hit(r, t_lo, t_hi, hist);
    bool hit_right = rchild->ray_hit(r, t_lo, hit_left ? hist.t : t_hi, hist);
    
    return hit_left || hit_right;
}

AABB BoundingVolumeNode::bounding_volume() const {
    return aabb;
}

void BoundingVolumeNode::translate(const vec3& offset) {
    if (lchild != nullptr) {
        lchild->translate(offset);
    }

    if (rchild != nullptr) {
        rchild->translate(offset);
    }

    aabb = AABB(lchild->bounding_volume(), rchild->bounding_volume());
}

void BoundingVolumeNode::rotate(double theta, char axis) {
    if (lchild != nullptr) {
        lchild->rotate(theta, axis);
    }

    if (rchild != nullptr) {
        rchild->rotate(theta, axis);
    }

    aabb = AABB(lchild->bounding_volume(), rchild->bounding_volume());
}