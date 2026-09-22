#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include <fstream>
#include <iostream>
#include <vector>

#include "rtweekend.h"
#include "material.h"
#include "light.h"

class camera {
  public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width  = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    point3 lookfrom = point3(0,0,0);   // Point camera is looking from
    point3 lookat   = point3(0,0,-1);  // Point camera is looking at
    vec3   vup      = vec3(0,1,0);     // Camera-relative "up" direction

    double vfov = 90;  // Vertical view angle (field of view)

    void render(const hittable& world, const std::vector<light>& lights, const color& ambient_light, const color& background_color) {
        initialize();

        // Open a file for writing
        std::ofstream outfile("output.ppm");

        outfile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            //std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; i++) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, world, lights, ambient_light, background_color, false);
                }
                write_color(outfile, pixel_samples_scale * pixel_color); // Write to file
            }
        }

        // Close the file
        outfile.close();

        //std::clog << "\rDone.                 \n";
    }

  private:
    int    image_height;   // Rendered image height
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;              // Camera frame basis vectors

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions.
        auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focal_length;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focal_length * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    vec3 reflect(const vec3& v, const vec3& n) {
        return v - 2 * dot(v, n) * n;
    }

    color ray_color(const ray& r, const hittable& world, const std::vector<light>& lights, const color& ambient_light, const color& background_color, bool reflected) {
        hit_record record;
    
        // If the ray hits an object in the world
        if (world.hit(r, interval(0, infinity), record)) {
            vec3 normal = record.normal; // Surface normal at the hit point
            vec3 hit_point = r.at(record.t); // Hit point in 3D space
    
            // View direction (from hit point to camera)
            vec3 view_dir = unit_vector(r.origin() - hit_point);

            vec3 hit_ray_origin = hit_point + normal * 0.001; // Bias to avoid self-intersection

            // Ambient component
            color result = ambient_light * record.material.ka * record.material.od;

            // Add the direct contribution from each visible light.
            for (const light& light_source : lights) {
                vec3 light_dir;
                double shadow_max = infinity;
                double attenuation = 1.0;

                if(!light_source.is_point()){
                    light_dir = unit_vector(light_source.direction());
                }
                else{
                    vec3 to_light = light_source.position() - hit_point;
                    double light_distance = to_light.length();
                    if (light_distance <= 0.001) {
                        continue;
                    }

                    light_dir = to_light / light_distance;
                    shadow_max = light_distance;
                    attenuation = 1.0 / (light_distance * light_distance);
                }

                ray shadow_ray(hit_ray_origin, light_dir);
                hit_record shadow_record;

                if (world.hit(shadow_ray, interval(0.001, shadow_max), shadow_record)) {
                    continue;
                }

                // Diffuse component
                double diffuse_intensity = std::max(dot(normal, light_dir), 0.0);
                result += attenuation * light_source.intensity() * record.material.kd
                    * record.material.od * diffuse_intensity;

                // Specular component
                vec3 reflect_dir = reflect(-light_dir, normal);
                double specular_intensity = std::pow(
                    std::max(dot(reflect_dir, view_dir), 0.0), record.material.kgls);
                result += attenuation * light_source.intensity() * record.material.ks
                    * record.material.os * specular_intensity;
            }

            color reflection_ray_color(0,0,0);

            //if material is reflective, then send a reflected ray
            if((record.material.refl > 0.0) && !reflected){
                ray reflected_ray(hit_ray_origin, reflect(r.direction(), normal));
                hit_record reflected_record;
                if(world.hit(reflected_ray, interval(0, infinity), reflected_record)){
                    reflection_ray_color = ray_color(reflected_ray, world, lights, ambient_light, background_color, true);
                }
                else{
                    reflection_ray_color = background_color;
                }
            }

            return result + record.material.refl * reflection_ray_color;
        }
    
        // If the ray doesn't hit anything, return the background color
        return background_color;
    }

    
};

#endif