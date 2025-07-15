#ifndef SPHERE_H
#define SPHERE_H

#include "objs.h"

// Child class of objs
class sphere : public objs {
    public:
        sphere(double rad, const vec3 &cen, shared_ptr<material> mat);
        bool ray_hit(const ray& r, double t_lo, double t_hi, hit_history &hist) override;
        AABB bounding_volume() const override;
        void translate(const vec3& offset) override;
        void rotate(double theta, char axis) override;

    private:
        double  radius;
        vec3    center;
        AABB    aabb;
        shared_ptr<material> material_;

        // Rotation angles for textures
        double x_theta = 0;
        double y_theta = 0;
        double z_theta = 0;
};

#endif