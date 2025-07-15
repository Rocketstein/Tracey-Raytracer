#ifndef COLOR_H
#define COLOR_H
#include <iostream>
#include <fstream>
#include "vec3.h"
#include "util.h"


using color = vec3;
void convert_to_255_scale(color &col);

// Note: sRGB values are in [0, 1]
double linear_to_srgb(double input);

#endif