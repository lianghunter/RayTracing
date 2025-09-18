#ifndef LIGHT_H
#define LIGHT_H

#include "vec3.h"
#include "color.h"

class light {
  public:
    light(const vec3& direction, const color& intensity)
        : light_direction(direction), light_intensity(intensity) {}

    // Get the direction of the light
    vec3 direction() const { return light_direction; }

    // Get the intensity of the light
    color intensity() const { return light_intensity; }

  private:
    vec3 light_direction;  // Direction of the light (normalized)
    color light_intensity; // Intensity of the light (white in this case)
};

#endif