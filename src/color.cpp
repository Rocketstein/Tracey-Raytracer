#include "color.h"

void convert_to_255_scale(color &col) {
    double lo = 0;
    double hi = 0.999;

    auto x = static_cast<int>(linear_to_srgb(utils::clamp(lo, hi, col.x())) * 256);
    auto y = static_cast<int>(linear_to_srgb(utils::clamp(lo, hi, col.y())) * 256);
    auto z = static_cast<int>(linear_to_srgb(utils::clamp(lo, hi, col.z())) * 256);
    

    double a = col.get_alpha();

    col = vec3(x, y, z, a);
}

double linear_to_srgb(double input) {
    if (input <= 0.0031308) {
        return 12.92 * input;
    } else {
        return 1.055 * pow(input, 1/2.4) - 0.055;
    }
}