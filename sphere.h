#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "material.h"

class sphere : public hittable {
    public:
        // Stationary Sphere
        sphere(const point3& static_center, double radius, shared_ptr<material> mat) 
        : center(static_center, vec3(0,0,0)), radius(std::fmax(0,radius)), mat(mat) {
            auto rvec = vec3(radius, radius, radius);
            bbox = aabb(static_center - rvec, static_center + rvec);
        }

        // Moving Sphere
        sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat) 
        : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat(mat) {
            auto rvec = vec3(radius, radius, radius);
            auto bbox1 = aabb(center.at(0) - rvec, center.at(0) + rvec);
            auto bbox2 = aabb(center.at(1) - rvec, center.at(1) + rvec);
            bbox = aabb(bbox1, bbox2);
        }

    aabb bounding_box() const override { return bbox; }

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
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    };

    private:
        ray center;
        double radius;
        shared_ptr<material> mat;
        aabb bbox;

        static void get_sphere_uv(const point3& p, double& u, double& v) {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

            auto theta = std::acos(-p.y());
            auto phi = std::atan2(-p.z(), p.x()) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }
};

#endif