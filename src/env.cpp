#include "env.h"

camera::camera(int width, int height, std::vector<unsigned char>& image, double fov, double dof_angle, const vec3& default_color, double aa_factor, double max_depth) : image_width(width),
                                                                                                                                    image_height(height),
                                                                                                                                    fov(fov),
                                                                                                                                    defocus_angle(dof_angle),
                                                                                                                                    scene_color(default_color),
                                                                                                                                    aa_factor(aa_factor),
                                                                                                                                    depth(max_depth), image(image) {}

double camera::get_emission_sx(double x, double y) {
    return (2 * ((x + 0.5)/(image_width) - 0.5)) * tan(deg_to_rad(fov/2)) * aspect_ratio;
}

double camera::get_emission_sy(double x, double y) {
    return (2 * (0.5 - ((y + 0.5) / image_height))) * tan(deg_to_rad(fov / 2));
}

color camera::ray_color(const ray& r, objs &world_list, int depth_level) const {
    if (depth_level >= depth) {
        return color(0, 0, 0);
    }
    hit_history hist;

    if (world_list.ray_hit(r, 1e-4, std::numeric_limits<double>::infinity(), hist)) {
        auto attenuation_secondary = hist.material_->scatter(r, hist.normal, hist.intersection, hist.is_front, hist.u, hist.v);
        auto attenuation = std::get<0>(attenuation_secondary);
        auto secondary   = std::get<1>(attenuation_secondary);
        auto emission    = hist.material_->emit(hist.intersection);

        if (hist.material_->is_emissive()) {
            // Check if the material is a light source using a sentinel vector
            return emission;
        }

        auto rec = ray_color(secondary, world_list, depth_level + 1);
        return vec3(rec.x() * attenuation.x(), rec.y() * attenuation.y(), rec.z() * attenuation.z()) + emission;
    }

    return scene_color;
}

void camera::preprocess(vec3 cam_pos, vec3 vision_pos, vec3 cam_up) {
    // Camera dimension setup
    camera_pos          = cam_pos;
    focal_length        = (cam_pos - vision_pos).magnitude();
    viewport_height     = 2 * tan(deg_to_rad(fov)/2) * focal_length;
    viewport_width      = viewport_height * (double(image_width)/image_height);
    g_forward           = (cam_pos - vision_pos).unit_vector();
    g_right             = cross(cam_up, g_forward).unit_vector();
    g_up                = cross(g_forward, g_right);
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.x() * p.x() + p.y() * p.y() < 1)
            return p;
    }
}

void camera::render(const world& w, const vec3& cam, const vec3& look) {
    preprocess(cam, look, vec3(0,1,0));
    world_list = w;

    const int channels = 3;
    const int stride = image_width * channels;

    // OpenMP parallel rendering
    #pragma omp parallel for schedule(dynamic, 1)
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            color c = vec3(0, 0, 0);
            
            if (aa_factor == 1) {
                double sx = get_emission_sx(i, j);
                double sy = get_emission_sy(i, j);
                vec3 direction_camera = vec3(sx, sy, -1);
                vec3 direction_world = (
                    direction_camera.x() * g_right +
                    direction_camera.y() * g_up +
                    direction_camera.z() * g_forward
                ).unit_vector();
                ray r(camera_pos, direction_world);
                c = ray_color(r, world_list, 0);
            } else {
                for (int k = 0; k < aa_factor; k++) {
                    double u = (i + utils::random_double(0, .5));
                    double v = (j + utils::random_double(0, .5));
                    double sx = get_emission_sx(u, v);
                    double sy = get_emission_sy(u, v);

                    vec3 direction_camera = vec3(sx, sy, -1);
                    vec3 direction_world = (
                        direction_camera.x() * g_right +
                        direction_camera.y() * g_up +
                        direction_camera.z() * g_forward
                    ).unit_vector();

                    vec3 ray_o = camera_pos;
                    vec3 ray_d = direction_world;

                    if (defocus_angle > 0) {
                        vec3 focal_point = camera_pos + focus_dist * ray_d;
                        auto rand_vec = random_in_unit_disk();
                        ray_o += rand_vec.x() * (g_right * defocus_radius) + rand_vec.y() * (g_up * defocus_radius);
                        ray_d = (focal_point - ray_o).unit_vector();
                    }

                    ray r(ray_o, ray_d);
                    c += ray_color(r, world_list, 0);
                }
                c /= aa_factor;
            }

            convert_to_255_scale(c);
            int pixel_index = (j * image_width + i) * channels;
            image[pixel_index + 0] = static_cast<unsigned char>(c.x());
            image[pixel_index + 1] = static_cast<unsigned char>(c.y());
            image[pixel_index + 2] = static_cast<unsigned char>(c.z());
        }

        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
    }

    std::clog << "\rDone.                 \n";
}

int camera::export_image(const std::vector<unsigned char>& image, int image_width, int image_height, int stride) {
    int channels = 3;
    std::time_t result = std::time(nullptr);
    std::asctime(std::localtime(&result));
    std::string filename = "output_" + std::to_string(result) + ".png";
    if (stbi_write_png(filename.c_str(), image_width, image_height, channels, image.data(), stride)) {
        std::cout << "Image saved as " << filename << std::endl;;
        return 1;
    } else {
        std::cerr << "Failed to save as image.\n";
        return 0;
    }
}