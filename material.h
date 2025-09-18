#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

struct Material {
    double kd; // Diffuse coefficient
    double ks; // Specular coefficient
    double ka; // Ambient coefficient
    color od;  // Diffuse color
    color os;  // Specular color
    double kgls; // Shininess exponent
    double refl; // Reflective material
};

#endif