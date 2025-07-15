#include "texture.h"

DefaultTexture::DefaultTexture(double size, const vec3& a, const vec3& b) : boxsize(size), col_A(a), col_B(b) {}

vec3 DefaultTexture::get_color_at(double u, double v, const vec3& point) const {
    using std::floor;
    
    auto sum =  static_cast<int>(floor(point.x() / boxsize)) + 
                static_cast<int>(floor(point.y() / boxsize)) + 
                static_cast<int>(floor(point.z() / boxsize));
    
    if (sum % 2 == 0) {
        return col_A;
    } else {
        return col_B;
    }
}

//__________________________________________________________________


ImageTexture::ImageTexture(const std::string& fpath) : image(utils::load_texture(fpath)) {}

vec3 ImageTexture::get_color_at(double u, double v, const vec3& point) const {
    if (image.data.empty()) {
        // If failed to load texture data, return magenta
        return vec3(1, 0, 1);
    }
    auto tex_x = static_cast<int>(utils::clamp(0, 1, u) * image.width - 1);
    auto tex_y = static_cast<int>((1 - utils::clamp(0, 1, v)) * image.height - 1);
    int idx = (tex_y * image.width + tex_x) * image.channels;
    int tex_r = image.data[idx];
    int tex_g = image.data[idx + 1];
    int tex_b = image.data[idx + 2];

    vec3 col(tex_r, tex_g, tex_b);
    return col / 255;
}