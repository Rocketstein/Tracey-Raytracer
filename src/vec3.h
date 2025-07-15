#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include "util.h"

/*
    Wrapper class for xyz coordinate data and operations.
*/

class vec3 {
    private:
        // Current location on a XYZ-plane. RGB for colors.
        double xyz[3];

        // Only for colors. -1 implies a vector coordinate.
        double alpha = -1;

    public:
        // constructors
        vec3();
        vec3(double x, double y, double z);
        vec3(double r, double g, double b, double a);

        // piecewise getters
        double x() const;
        double y() const;
        double z() const;
        double get_alpha() const;
        void set_alpha(double value);

        // local operators (for convenience)
        vec3 operator+=(const vec3 &vec) {
            for (int i = 0; i < 3; i++) {
                xyz[i] += vec.xyz[i];
            }

            return *this;
        }
        vec3 operator-=(const vec3 &vec) {
            for (int i = 0; i < 3; i++) {
                xyz[i] -= vec.xyz[i];
            }

            return *this;
        }
        vec3 operator*=(double k) {
            for (int i = 0; i < 3; i++) {
                xyz[i] *= k;
            }

            return *this;
        }
        vec3 operator/=(double k) {
            for (int i = 0; i < 3; i++) {
                xyz[i] /= k;
            }

            return *this;
        }

        inline friend vec3 operator+(const vec3 &vec_1, const vec3 &vec_2) {
            return vec3(vec_1.xyz[0] + vec_2.xyz[0],
                        vec_1.xyz[1] + vec_2.xyz[1],
                        vec_1.xyz[2] + vec_2.xyz[2]);
        }
        inline friend vec3 operator+(const vec3 &vec_1, double k) {
            return vec3(vec_1.xyz[0] + k,
                        vec_1.xyz[1] + k,
                        vec_1.xyz[2] + k);
        }
        inline friend vec3 operator-(const vec3 &vec_1, const vec3 &vec_2) {
            return vec3(vec_1.xyz[0] - vec_2.xyz[0],
                        vec_1.xyz[1] - vec_2.xyz[1],
                        vec_1.xyz[2] - vec_2.xyz[2]);
        }
        inline friend vec3 operator-(const vec3 &vec_1, double k) {
            return vec3(vec_1.xyz[0] - k,
                        vec_1.xyz[1] - k,
                        vec_1.xyz[2] - k);
        }
        inline vec3 friend operator*(const vec3 &vec_1, double k) {
            return vec3(vec_1.xyz[0] * k,
                        vec_1.xyz[1] * k,
                        vec_1.xyz[2] * k);
        }
        inline vec3 friend operator*(double k, const vec3 &vec_1) {
            return vec_1 * k;
        }

        // Equivalent to dot product
        inline double friend operator*(const vec3 &vec_1, const vec3 &vec_2) {
            return (vec_1.xyz[0] * vec_2.xyz[0] + vec_1.xyz[1] * vec_2.xyz[1] + vec_1.xyz[2] * vec_2.xyz[2]);
        }
        inline friend vec3 operator/(const vec3 &vec_1, double k) {
            return vec3(vec_1.xyz[0] / k,
                        vec_1.xyz[1] / k,
                        vec_1.xyz[2] / k);
        }

        // Utils
        // Returns the Euclidean magnitude of the vector
        double magnitude() const;

        // Prints the current coordinate of the vector
        void print() const;

        // Returns the unit of the current vector
        vec3 unit_vector() const;
};

// Returns a random vector in the spcecified sphere space
vec3 random_vector(double radius);


// Returns the cross product of the two vectors
vec3 cross(vec3 &vec_1, vec3 &vec_2);


#endif