#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "material.h"

class sphere : public hittable {
    public:
    // Stationary Sphere
    sphere(const point3& static_center, double radius, shared_ptr<material> mat)
      : center(static_center, vec3(0,0,0)), radius(std::fmax(0,radius)), mat(mat) {}

    // Moving Sphere
    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat)
      : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat(mat) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto current_center = center.at(r.time());
        auto oc = current_center - r.origin();
        double a = dot(r.direction(), r.direction());
        double h = dot(r.direction(), oc);
        double c = dot(oc, oc) - radius*radius;

        double discriminant = h*h - a*c;
        if (discriminant < 0) {
            return false;
        }
        double sqrtd = std::sqrt(discriminant);
        double root = (h - sqrtd) / a; 

        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.point = r.at(root);
        auto outward_normal = (rec.point - current_center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    };

    private:
        ray center;
        double radius;
        shared_ptr<material> mat;
};

#endif