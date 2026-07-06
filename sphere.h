#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "material.h"

class sphere : public hittable {
    public:
    sphere(const point3& center, double radius, shared_ptr<material> mat): center(center), radius(std::fmax(0,radius)), mat(mat) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        auto oc = center - r.origin();
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
        auto outward_normal = (rec.point - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    };

    private:
        point3 center;
        double radius;
        shared_ptr<material> mat;
};

#endif