#ifndef CAMERA_H
#define CAMERA_H

#include "vector3.h"
#include "color.h"
#include "material.h"
#include <thread>
#include <mutex>
std::mutex write_mutex;

class Camera {
  public:
    
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 20;
    int max_depth = 10;       // Max ray bounces
    Vector3 background; // This is the background

    double vfov = 90;  // Vertical view angle (field of view)
    Vector3 lookfrom = Vector3(0,0,0);   // Point camera is looking from
    Vector3 lookat   = Vector3(0,0,-1);  // Point camera is looking at
    Vector3 vup      = Vector3(0,1,0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    const Vector3& getPosition()    const { return position; }
    double getViewport_width()      const { return image_width; }
    double getViewport_height()     const { return image_height; }
    double getAspect_ratio()        const { return aspect_ratio; }

    Camera(std::ofstream& ofs, int aa, const cube_map_texture& cube_map) : ofs(ofs), aa(aa), cube_map(cube_map) {}

    void render_section(int start_r, int end_r, const hittable& world, std::vector<Vector3>& framebuffer)
    {
      for(int h = start_r; h < end_r; h++)
      {
        for(int w = 0; w < image_width; w++)
        {
          Vector3 pixel_color(0,0,0);
          Vector3 q;
          for(int i = 0; i < aa; i++)
          {

            Vector3 offset = sample_square();  // Get random offset in [-0.5, 0.5] range
            q[0] = w + (offset[0] / image_width);  // Apply horizontal jitter
            q[1] = h + (offset[1] / image_height); // Apply vertical jitter

            //Send out raysence between the camera's position and the targeted pixels location
            Vector3 cur_pixel_location = leftmost_pixel + (q[0] * pixel_delta_u) + (q[1] * pixel_delta_v);
            // Vector3 cur_pixel_location = leftmost_pixel + q[0] * pixel_delta_u + q[1] * pixel_delta_v;
            Vector3 ray_origin = (defocus_angle <= 0) ? position : defocus_disk_sample();
            //Vector3 ray_direction = getUnit_Vector(cur_pixel_location - position);
            Vector3 ray_direction = cur_pixel_location - ray_origin;
            
            double ray_time = random_double();
            Ray cur_ray(position, ray_direction, ray_time);

            pixel_color = pixel_color + ray_color(cur_ray, max_depth, world, cube_map);
          }
          pixel_color = pixel_color / aa;
          framebuffer[h * image_width + w] = pixel_color;
          // {
          //       std::lock_guard<std::mutex> lock(write_mutex);
          //       write_color(ofs, pixel_color);
          // }
          
          processed_pixels++;
          if (processed_pixels % (total_pixels / 100) == 0) {
              int progress = (processed_pixels * 100) / total_pixels;
              std::cout << "\rProgress: " << progress << "%";
              std::cout.flush();
          }
        }
      }
    }

    void render(const hittable& world, int num_threads)
    {

      initialize();
      
      std::vector<Vector3> framebuffer(image_width * image_height);

      // int columns_per_thread = (image_width + num_threads - 1) / num_threads;
      int rows_per_thread = (image_height + num_threads - 1) / num_threads;
      std::vector<std::thread> threads;
      threads.reserve(num_threads);

      for (int j = 0; j < num_threads; j++) {
          int start_col = j * rows_per_thread;
          int end_col = std::min(start_col + rows_per_thread, image_height); 

          if (start_col >= image_width) break; 

          threads.emplace_back(&Camera::render_section, this, start_col, end_col, std::ref(world), std::ref(framebuffer));
      }

      for (auto& thread : threads) {
          thread.join();
      }

      for (int h = 0; h < image_height; h++) {
        for (int w = 0; w < image_width; w++) {
            write_color(ofs, framebuffer[h * image_width + w]);
        }
      }

      std::cout << std::endl << "Processed Pixels: " << processed_pixels << " / " << (image_width * image_height) << std::endl;
      
    }

  private:
    
    int image_height;
    int total_pixels;
    int processed_pixels = 0;
    Vector3 position;
    const cube_map_texture& cube_map;
    double pixel_samples_scale;
    Vector3 leftmost_pixel;
    Vector3 pixel_delta_u;
    Vector3 pixel_delta_v;
    Vector3 u, v, w;              // Camera frame basis vectors
    std::ofstream& ofs;
    int aa;                       // Antialisaing value
    Vector3 defocus_disk_u;       // Defocus disk horizontal radius
    Vector3 defocus_disk_v;       // Defocus disk vertical radius

  Vector3 sample_square() const {
    // Returns the vector to a random point in the [-0.5, -0.5] to [+0.5, +0.5] unit square.
    return Vector3((random_double() - 0.5), (random_double() - 0.5), 0);
  }

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;
    total_pixels = image_height * image_width;
    processed_pixels = 0;

    ofs << "P3\n"; // P3 for colors in ASCII
    ofs << image_width << " " << image_height << "\n"; // Determine image size
    ofs << "255\n"; // Max color value

    position = lookfrom;
    //double focal_length = (lookfrom - lookat).getLength();
    auto theta = degrees_radians(vfov);
    auto h = std::tan(theta/2);
    auto viewport_height = 2 * h * focus_dist;
    //double viewport_height = 2 * h * focal_length;
    double viewport_width = viewport_height * (double(image_width)/image_height);

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = getUnit_Vector(lookfrom - lookat);
    // vup just has to be on the same plane as w in order to make u orthogonal vector
    u = getUnit_Vector(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    Vector3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
    Vector3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

    pixel_samples_scale = 1.0 / samples_per_pixel;

    pixel_delta_u = viewport_u / (image_width * 1.0);
    pixel_delta_v = viewport_v / (image_height * 1.0);

    Vector3 viewport_up_leftside = position - (focus_dist  * w) - (viewport_u / 2) - (viewport_v / 2);
    leftmost_pixel = viewport_up_leftside + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    auto defocus_radius = focus_dist * std::tan(degrees_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  
  }

  Vector3 ray_color( const Ray& r, int depth, const hittable& world, const cube_map_texture& cube_map)
  {

    // if(depth <= 0)
    // {
    //   return Vector3(0,0,0);
    // }

    // hit_record rec;
  
    // if (!world.hit(r, interval(0.001, infinity), rec))
    //     return background;

    // Ray scattered;
    // Vector3 attenuation;
    // Vector3 color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

    // if (!rec.mat->scatter(r, rec, attenuation, scattered))
    //     return color_from_emission;

    // Vector3 color_from_scatter = multiply(attenuation, ray_color(scattered, depth-1, world));
    // // double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);
    // // double pdf_value = scattering_pdf;

    // // Vector3 color_from_scatter = (scattering_pdf * multiply(attenuation, ray_color(scattered, depth-1, world))) / pdf_value;

    // return color_from_emission + color_from_scatter;

    if (depth <= 0)
      return Vector3(0, 0, 0); // No light is gathered

    hit_record rec;

    // If the ray hits an object, calculate the color
    if (world.hit(r, interval(0.001, infinity), rec)) {
        Ray scattered;
        Vector3 attenuation;
        Vector3 color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);
        // if (rec.mat->scatter(r, rec, attenuation, scattered)) {
        //     return multiply(attenuation, ray_color(scattered, depth - 1, world, cube_map));
        // }
        // return Vector3(0, 0, 0); // Absorbed
        if (rec.mat->scatter(r, rec, attenuation, scattered)) {
            double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

            if (scattering_pdf > 0) {
                double pdf_value = scattering_pdf;
                Vector3 color_from_scatter = multiply(attenuation, ray_color(scattered, depth - 1, world, cube_map) * scattering_pdf) / pdf_value;

                return color_from_emission + color_from_scatter;
            }
        }
        return color_from_emission;
    }

    // If the ray hits nothing, sample from the cube map
    Vector3 environment_light = cube_map.value(0, 0, getUnit_Vector(r.getDirection())) * 1.5;
    return environment_light;

  }

  Vector3 defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk.
    Vector3 p = random_in_unit_disk();
    return position + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }
    
};

#endif 