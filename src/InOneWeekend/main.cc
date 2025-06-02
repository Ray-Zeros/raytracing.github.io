//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include <string>
#include <chrono>

void usage(const char* program_name) {
    std::cerr << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  -w <width>      Image width\n"
              << "  -s <samples>    Samples per pixel\n"
              << "  -d <depth>      Max ray depth\n"
              << "  -t <threads>    Thread count (default: 0, use system default)\n"
              << "  -h              Show this help message\n";
}

int main(int argc, char* argv[]) {
    // Default parameters
    int image_width = 400;
    int samples_per_pixel = 100;
    int max_depth = 50;
    int num_threads = 0;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h") {
            usage(argv[0]);
            return 0;
        } else if (arg == "-w" && i + 1 < argc) {
            image_width = std::stoi(argv[++i]);
        } else if (arg == "-s" && i + 1 < argc) {
            samples_per_pixel = std::stoi(argv[++i]);
        } else if (arg == "-d" && i + 1 < argc) {
            max_depth = std::stoi(argv[++i]);
        } else if (arg == "-t" && i + 1 < argc) {
            num_threads = std::stoi(argv[++i]);
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            usage(argv[0]);
            return 1;
        }
    }

    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.num_threads       = num_threads;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    std::clog << "Starting render...\n";
    std::clog << "Image size: " << image_width << "x" << int(image_width / cam.aspect_ratio) << "\n";
    std::clog << "Samples per pixel: " << samples_per_pixel << "\n";
    std::clog << "Max ray depth: " << max_depth << "\n";

    auto render_start_time = std::chrono::high_resolution_clock::now();
    
    cam.render(world);

    auto render_time = std::chrono::high_resolution_clock::now() - render_start_time;
    std::clog << "Render completed in " 
              << std::chrono::duration_cast<std::chrono::seconds>(render_time).count() 
              << " seconds\n";
    
    return 0;
}
