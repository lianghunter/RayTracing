// triangle.h
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

class triangle : public hittable {
  public:
    triangle(point3 v0, point3 v1, point3 v2) 
        : vert0(v0), vert1(v1), vert2(v2) {
            // Precompute normal during construction
            edge1 = vert1 - vert0;
            edge2 = vert2 - vert0;
            normal = unit_vector(cross(edge1, edge2));
        }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        vec3 h = cross(r.direction(), edge2);
        float a = dot(edge1, h);
        if (a > -0.0001 && a < 0.0001) // Ray parallel to triangle
            return false;

        float f = 1.0 / a;
        vec3 s = r.origin() - vert0;
        float u = f * dot(s, h);
        
        if (u < 0.0 || u > 1.0)
            return false;

        vec3 q = cross(s, edge1);
        float v = f * dot(r.direction(), q);
        
        if (v < 0.0 || u + v > 1.0)
            return false;

        float t = f * dot(edge2, q);
        
        if (!ray_t.surrounds(t))
            return false;

        rec.t = t;
        rec.p = r.at(rec.t);
        rec.set_face_normal(r, normal);
        rec.material = this->material;
        return true;
    }

  private:
    point3 vert0, vert1, vert2;
    vec3 edge1, edge2;
    vec3 normal;
};
#endif