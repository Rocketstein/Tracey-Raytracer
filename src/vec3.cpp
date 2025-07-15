#include "vec3.h"
#define _USE_MATH_DEFINES

// Constructors
vec3::vec3() : xyz{0, 0, 0} { }
vec3::vec3(double x, double y, double z) : xyz{x, y, z} {}
vec3::vec3(double r, double g, double b, double a) : xyz{r, g, b}, alpha(a) {}

// Getters / Setters
double vec3::x() const {
    return xyz[0];
}
double vec3::y() const {
    return xyz[1];
}
double vec3::z() const {
    return xyz[2];
}
double vec3::get_alpha() const {
    return alpha;
}
void vec3::set_alpha(double value) {
    alpha = value;
}

// Utils
double vec3::magnitude() const {
    return sqrt(pow(xyz[0], 2) + pow(xyz[1], 2) + pow(xyz[2], 2));
}
void vec3::print() const {
    std::cout << "x: " << xyz[0] << "y: " << xyz[1] << "z: " << xyz[2] << std::endl; 
}

vec3 cross(vec3 &vec_1, vec3 &vec_2) {
    auto x1 = vec_1.x();
    auto x2 = vec_2.x();
    auto y1 = vec_1.y();
    auto y2 = vec_2.y();
    auto z1 = vec_1.z();
    auto z2 = vec_2.z();

    return vec3(y1 * z2 - z1 * y2,
                z1 * x2 - x1 * z2,
                x1 * y2 - y1 * x2);
}

vec3 vec3::unit_vector() const {
    double length = magnitude();
    return vec3(xyz[0] / length, xyz[1] / length, xyz[2] / length);
}

vec3 random_vector(double radius) {
    double u = utils::random_double(0, radius);
    double theta = utils::random_double(0.0, 2 * M_PI); // azimuthal angle
    double phi = acos(1 - 2 * utils::random_double(0.0, 1.0)); // polar angle (correct for uniformity)

    double x = sin(phi) * cos(theta);
    double y = sin(phi) * sin(theta);
    double z = cos(phi);
    
    return vec3(x, y, z);
}