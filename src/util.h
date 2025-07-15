#ifndef UTIL_H
#define UTIL_H

#include <cmath>
#include <random>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <thread>

#include "lib/stb_image.h"

#define EPSILON 1e-6

struct Image {
    std::vector<unsigned char> data;
    int width       = 0;
    int height      = 0;
    int channels    = 0;
};

namespace utils {
    // Generates a random double in range [x, y)
    double random_double(double x, double y);

    // Clamps the input value within the desired range
    double clamp(double lo, double hi, double val);

    // Converts degree to equivalent radians
    double deg_to_rad(double deg);

    // Loads a texture from a given image file
    Image load_texture(const std::string& fpath);

    // Mitchell-Netravali AA-filter
    double mitchell_filter(double x);
}
#endif