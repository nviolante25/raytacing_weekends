#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "rtweekend.h"
#include "material.h"

class camera {
    public:
        int image_width=400;
        double aspect_ratio = 16.0 / 9.0;
        int samples_per_pixel = 100;
        int max_depth = 10;

        double vfov = 90;  // Vertical view angle (field of view)
        point3 lookfrom = point3(0,0,0);   // Point camera is looking from
        point3 lookat   = point3(0,0,-1);  // Point camera is looking at
        vec3   vup      = vec3(0,1,0);     // Camera-relative "up" direction

        double defocus_angle = 0;  // Variation angle of rays through each pixel
        double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

        void render(const hittable& world) {
            initialize();
        
            std::cout << "P3" << std::endl;
            std::cout << image_width << ' ' << height << std::endl;
            std::cout << 255 << std::endl;
            for (int row=0; row<height; row++) {
                std::clog << "\rScanlines remaning: " << (height - row) << std::flush;
                for (int col=0; col<image_width; col++) {
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
        vec3   defocus_disk_u;       // Defocus disk horizontal radius
        vec3   defocus_disk_v;       // Defocus disk vertical radius

    void initialize() {
        height = int(image_width / aspect_ratio);
        camera_center = lookfrom;
        pixels_sample_scale = 1.0 / samples_per_pixel;

        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * double(image_width) / double(height);

        w = unit_vector(lookfrom - lookat); // backward dir
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        delta_u = u * viewport_width / image_width;
        delta_v = - v * viewport_height / height;
        // This is actually very simple to understand
        // The top-left corner in the uvw coordinate system of the camera in viewport units is
        // (-viewport_width/2, viewport_height/2, 1)
        // The math to generate the pixel location in uvw coordinates is then
        // center - (-(viewport_width/2) * u + (viewport_height/2) * v + (1) * w)
        // Then we have the half pixel offset to center the pixel

        pixel00_loc = camera_center - viewport_width/2 * u + viewport_height/2 * v -  focus_dist * w + 0.5 * (delta_u + delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
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
    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return camera_center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
    ray get_ray(const int row, const int col) const {
        auto offset = sample_square();
        auto pixel_center = pixel00_loc + (col + offset.x()) * delta_u + (row + offset.y()) * delta_v;
        
        auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();

        auto ray_direction = pixel_center - ray_origin;
        ray r(ray_origin, ray_direction);
        return r;
    }
};

#endif