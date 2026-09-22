#ifndef LIGHT_H
#define LIGHT_H

#include "vec3.h"
#include "color.h"

enum class light_type {
    directional,
    point
};

class light {
  public:
    light(const vec3& direction, const color& intensity)
        : type_(light_type::directional), light_direction(direction),
          light_position(), light_intensity(intensity) {}

    light(light_type type, const point3& position, const color& intensity)
        : type_(type), light_direction(), light_position(position),
          light_intensity(intensity) {}

    // Get the direction of the light
    vec3 direction() const { return light_direction; }

    // Get the position of a point light
    point3 position() const { return light_position; }

    light_type type() const { return type_; }
    bool is_directional() const { return type_ == light_type::directional; }
    bool is_point() const { return type_ == light_type::point; }

    // Get the intensity of the light
    color intensity() const { return light_intensity; }

  private:
    light_type type_;
    vec3 light_direction;  // Direction of the light (normalized)
    point3 light_position; // Position of the point light
    color light_intensity; // Intensity of the light (white in this case)
};

#endif