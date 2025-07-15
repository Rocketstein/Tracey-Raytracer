#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <vector>
#include "../vec3.h"
#include "../util.h"
#include "../lib/stb_image_write.h"

using std::shared_ptr, std::vector;

class Texture {
    public:
        virtual ~Texture() = default;

        // Retrieves the color emitted by the texture at the specified uv coordinate
        virtual vec3 get_color_at(double u, double v, const vec3& point) const = 0;
};

class DefaultTexture : public Texture {
    public:
        DefaultTexture(double boxsize, const vec3& a, const vec3& b);

        vec3 get_color_at(double u, double v, const vec3& point) const override;
        
    private:
        double boxsize;
        vec3 col_A;
        vec3 col_B;
};

class ImageTexture : public Texture {
    public:
        ImageTexture(const std::string& fpath);

        vec3 get_color_at(double u, double v, const vec3& point) const override;
    private:
        Image image;
};


#endif