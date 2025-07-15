#ifndef DIFFUSE_H
#define DIFFUSE_H

#include <memory>
#include "material.h"
#include "../texture/texture.h"

using std::shared_ptr;

class diffuse : public material {
    public:
        // Diffuse surface
        diffuse(const vec3& alb);

        // Texture surface
        diffuse(shared_ptr<Texture> tex);

        // Format: tuple<attenuation, resulting secondary ray>
        tuple<vec3, ray> scatter(const ray &r, const vec3& normal, const vec3& intersection, bool front, double u, double v) const override;

    private:
        bool use_textures = false;
        shared_ptr<Texture> texture;

};

#endif