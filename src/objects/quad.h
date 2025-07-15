#ifndef QUAD_H
#define QUAD_H

#include "objs.h"

class Quad : public objs {
    public:
        Quad(const vec3& q, const vec3& u, const vec3& v, shared_ptr<material> mat);

        bool ray_hit(const ray& r, double t_lo, double t_hi, hit_history &hist) override;

        AABB bounding_volume() const override;

        void translate(const vec3& offset) override;

        void rotate(double theta, char axis) override;

    private:
        vec3 cornerstone;   // Coordinate of the defining vertex
        vec3 u;
        vec3 v;
        vec3 normal;
        AABB aabb;
        shared_ptr<material> material_;
};

#endif