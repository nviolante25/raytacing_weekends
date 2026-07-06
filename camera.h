#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "rtweekend.h"
#include "material.h"

class camera {
    public:
        int width=400;
        double aspect_ratio = 16.0 / 9.0;
        int samples_per_pixel = 100;
        int max_depth = 10;

        double vfov = 90;  // Vertical view angle (field of view)
        point3 lookfrom = point3(0,0,0);   // Point camera is looking from
        point3 lookat   = point3(0,0,-1);  // Point camera is looking at
        vec3   vup      = vec3(0,1,0);     // Camera-relative "up" direction

        void render(const hittable& world) {
            initialize();
        
            std::cout << "P3" << std::endl;
            std::cout << width << ' ' << height << std::endl;
            std::cout << 255 << std::endl;
            for (int row=0; row<height; row++) {
                std::clog << "\rScanlines remaning: " << (height - row) << std::flush;
                for (int col=0; col<width; col++) {
                    color pixel_color(0,0,0);
                    for (int i=0; i<samples_per_pixel; i++) {
                        auto r = get_ray(row, col);
                        pixel_color += ray_color(r, world, 0);
                    }

                    write_color(std::cout, pixel_color * pixels_sample_scale);
                }
            }
            std::clog << "\rDone.                  \n";
        }

    private:
        int    height;   // Rendered image height
        point3 camera_center;         // Camera center
        point3 pixel00_loc;    // Location of pixel 0, 0
        vec3   delta_u;  // Offset to pixel to the right
        vec3   delta_v;  // Offset to pixel below
        double pixels_sample_scale;
        vec3   u, v, w;              // Camera frame basis vectors

    void initialize() {
        height = int(width / aspect_ratio);
        camera_center = lookfrom;
        pixels_sample_scale = 1.0 / samples_per_pixel;

        auto focal_lenght = 1;
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = viewport_height * double(width) / double(height);

        w = unit_vector(lookfrom - lookat); // backward dir
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        delta_u = u * viewport_width / width;
        delta_v = - v * viewport_height / height;
        pixel00_loc = camera_center + - viewport_width/2 * u + viewport_height/2 * v -  w + 0.5 * (delta_u + delta_v);
    }

    color ray_color(const ray& r, const hittable& world, int depth) {
        if (depth >= max_depth)
            return color(0,0,0);

        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation; 
            if (rec.mat->scatter(r, rec, attenuation, scattered)) 
                return attenuation * ray_color(scattered, world, depth+1);
            return color(0,0,0);
        }
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }
    ray get_ray(const int row, const int col) const {
        auto offset = sample_square();
        auto pixel_center = pixel00_loc + (col + offset.x()) * delta_u + (row + offset.y()) * delta_v;

        auto ray_direction = pixel_center - camera_center;
        ray r(camera_center, ray_direction);
        return r;
    }
};

#endif