#include "rtweekend.h"

#include <stdio.h>
#include <iostream>

#include "vec3.h"
#include "color.h"
#include "ray.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"


double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = center - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = h*h - a*c;

    if (discriminant < 0) {
        return -1.0;
    } else {
        return (h - std::sqrt(discriminant)) / a;
    }
}

color ray_color(const ray& r) {
    auto t = hit_sphere(point3(0,0,-1), 0.5, r);
    if (t>0) {
        auto normal = unit_vector(r.at(t) - point3(0,0,-1));
        return (0.5 * normal) + 0.5;
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

int main() {

    int width=400;
    double aspect_ratio = 16.0 / 9.0;
    int height = int(width / aspect_ratio);
    aspect_ratio = double(width) / double(height);

    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * aspect_ratio;

    auto focal_lenght = 1.0;
    auto camera_center = point3(0, 0, 0);

    auto delta_u = vec3(viewport_width / width, 0, 0);
    auto delta_v = vec3(0, -viewport_height / height, 0);
    auto pixel_00_loc = vec3(-aspect_ratio, 1, -1) + 0.5 * (delta_u + delta_v);


    // Print header
    std::cout << "P3" << std::endl;
    std::cout << width << ' ' << height << std::endl;
    std::cout << 255 << std::endl;
    for (int row=0; row<height; row++) {
        std::clog << "\rScanlines remaning: " << (height - row) << std::flush;
        for (int col=0; col<width; col++) {
            auto pixel_center = pixel_00_loc + col * delta_u + row * delta_v;
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                  \n";
}