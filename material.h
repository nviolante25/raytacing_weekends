#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "color.h"
#include "aabb.h"

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const {
        return false;
    }
};

class lambertian : public material {
    public:
    lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 scatter_direction = rec.normal + random_unit_vector(); // cosine weigthed
        
        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
        scatter_direction = rec.normal;
        
        scattered = ray(rec.point, scatter_direction, r_in.time());
        attenuation = albedo;
        return true;
    }
    private:
        color albedo;
};

class metal: public material {
    public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto scattered_direction = reflect(r_in.direction(), rec.normal);
        auto reflected = unit_vector(scattered_direction) + (fuzz * random_unit_vector());
        scattered = ray(rec.point, reflected, r_in.time());
        attenuation = albedo;

        return (dot(scattered.direction(), rec.normal) > 0);
    }
    private:
        color albedo;
        double fuzz;

};

class dielectric: public material {
    public:
    dielectric(const double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto unit_dir = unit_vector(r_in.direction());
        double etai_over_etat = rec.front_face ? 1.0/refraction_index: refraction_index;
        
        auto cos_theta1 = - dot(unit_dir, rec.normal);
        bool can_refract = etai_over_etat * std::sqrt(1 - std::fmin(cos_theta1 * cos_theta1, 1)) <= 1;
        vec3 direction;
        reflectance(cos_theta1, refraction_index);
        if (can_refract && reflectance(cos_theta1, etai_over_etat) <= random_double())
            direction = refract(unit_dir, rec.normal, etai_over_etat);
        else
            direction = reflect(unit_dir, rec.normal);
        scattered = ray(rec.point, direction, r_in.time());
        attenuation = color(1, 1, 1);
        return true;
    }

    private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;
    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};

#endif