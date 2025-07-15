#ifndef ENV_H
#define ENV_H

#include <omp.h>
#include <thread>
#include <atomic>
#include <vector>
#include <ctime>
#include "util.h"
#include "color.h"
#include "objects/world.h"
#include "lib/stb_image_write.h"

using std::tan;
using namespace utils;

// Hic sunt background stuffs
class camera {
    private:
        std::vector<unsigned char>&  image;
        world   world_list;
        int     image_width       = 600;
        int     image_height      = 600;
        double  focal_length;
        double  viewport_width;
        double  viewport_height;
        double  fov               = 40;
        double  defocus_angle     = 0.0;
        double  focus_dist        = 10;
        double  defocus_radius    = focus_dist * tan(deg_to_rad(defocus_angle/ 2));
        const double aspect_ratio = (double (image_width)) / image_height;

        vec3    scene_color       = vec3(0.0, 0.0, 0.0);                                // Default background color

        // Number of rays shot per pixel
        const double aa_factor    = 70;
        const int    depth        = 10;

        vec3 camera_pos    = vec3(0, 0, 0);
        vec3 g_forward     = vec3(0, 0, -1);
        vec3 g_right       = vec3(1, 0, 0);
        vec3 g_up          = vec3(0, 1, 0);

        double get_emission_sx(double x, double y);
        double get_emission_sy(double x, double y);

    public:
        camera(int width, int height, std::vector<unsigned char>& image, double fov, double dof_angle, const vec3& default_color, double aa_factor, double max_depth);
        void    render(const world& w, const vec3& cam_pos, const vec3& look_dir);
        color   ray_color(const ray& r, objs &world_list, int depth_level) const;
        void    preprocess(vec3 cam_pos, vec3 cam_look_dir, vec3 cam_up);
        int     export_image(const std::vector<unsigned char>& image, int image_width, int image_height, int stride);
}; 

#endif