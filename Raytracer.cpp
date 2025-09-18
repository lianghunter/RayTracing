#include <iostream>
#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "triangle.h"


#include <vector>
#include <string>
#include <fstream>

using namespace std;

int main() {

    light spotlight(vec3(1, 0, 0), color(1,1,1));
    color ambient_light(0.1, 0.1, 0.1);
    color background_color(0.2, 0.2, 0.2);

    hittable_list world;

    sphere s1(point3(0.5, 0, -0.15), 0.05);
    s1.material = {0.8, 0.1, 0.3, color(1.0, 1.0, 1.0), color(1.0, 1.0, 1.0), 4.0, 0.0};
    world.add(make_shared<sphere>(s1));

    sphere s2(point3(0.3, 0, -0.1), 0.08);
    s2.material = {0.8, 0.8, 0.1, color(1.0, 0, 0), color(0.5, 1.0, 0.5), 32.0, 0.0};
    world.add(make_shared<sphere>(s2));

    sphere s3(point3(-0.6, 0, 0), 0.3);
    s3.material = {0.7, 0.5, 0.1, color(0, 1.0, 0), color(0.5, 1.0, 0.5), 64.0, 0.0};
    world.add(make_shared<sphere>(s3));

    sphere s4(point3(0.1, -0.55, 0.25), 0.3);
    s4.material = {0.0, 0.1, 0.1, color(0.75, 0.75, 0.75), color(1.0, 1.0, 1.0), 10.0, 0.9};
    world.add(make_shared<sphere>(s4));

    triangle t1(point3(0.3, -0.3, -0.4), point3(0.0, 0.3, -0.1), point3(-0.3, -0.3, 0.2));
    t1.material = {0.9, 0.9, 0.1, color(0.0, 0, 1.0), color(1.0, 1.0, 1.0), 32.0, 0.0};
    world.add(make_shared<triangle>(t1));

    triangle t2(point3(-0.2, 0.1, 0.1), point3(-0.2, -0.5, 0.2), point3(-0.2, 0.1, -0.3));
    t2.material = {0.9, 0.5, 0.1, color(1.0, 1.0, 0.0), color(1.0, 1.0, 1.0), 4.0, 0.0};
    world.add(make_shared<triangle>(t2));

    camera cam;

    cam.aspect_ratio      = 1.0/1.0;
    cam.image_width       = 800;
    cam.samples_per_pixel = 100;

    cam.lookat   = point3(0,0,0);
    cam.lookfrom = point3(0,0,1);
    cam.vup      = vec3(0,1,0);
    cam.vfov     = 90;

    cam.render(world, spotlight, ambient_light, background_color);
}