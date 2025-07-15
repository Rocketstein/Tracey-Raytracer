#include "util.h"

// double utils::random_double(double x, double y) {
//     static std::random_device rd;  // Seed
//     static std::mt19937 gen(rd()); // Mersenne Twister engine
//     std::uniform_real_distribution<> dis(x, y);
//     return dis(gen);
// }

double utils::random_double(double x, double y) {
    thread_local std::random_device rd;  // Seed (thread-local)
    thread_local std::mt19937 gen(rd()); // Mersenne Twister engine (thread-local)
    std::uniform_real_distribution<> dis(x, y);
    return dis(gen);
}

double utils::clamp(double lo, double hi, double val) {
    if (val < lo) {
        return lo;
    } else if (val > hi) {
        return hi;
    } else {
        return val;
    }
}

double utils::deg_to_rad(double degrees) {
    return degrees * M_PI / 180;
}

Image utils::load_texture(const std::string& fpath) {
    Image img;
    unsigned char* raw_data = stbi_load(fpath.c_str(), &img.width, &img.height, &img.channels, 0);
    if (!raw_data) {
        std::cerr << "Failed to open " << fpath << '\n';
        return img;
    }

    size_t size = img.width * img.height * img.channels;
    img.data.assign(raw_data, raw_data + size);
    stbi_image_free(raw_data);
    return img;
}

double utils::mitchell_filter(double x) {
    const double B = 1.0/3.0, C = 1.0/3.0;
    x = abs(x);
    if (x < 1.0) {
        return ((12 - 9*B - 6*C)*x*x*x + (-18 + 12*B + 6*C)*x*x + (6 - 2*B))/6.0;
    } else if (x < 2.0) {
        return ((-B - 6*C)*x*x*x + (6*B + 30*C)*x*x + (-12*B - 48*C)*x + (8*B + 24*C))/6.0;
    }
    return 0.0;
}