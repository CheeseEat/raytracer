#ifndef CAMERA_H
#define CAMERA_H

#include "vector3.h"
#include "color.h"
#include "material.h"

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

    Camera(std::ofstream& ofs, int aa) : ofs(ofs), aa(aa) {}

    void render(const hittable& world)
    {

      initialize();

      int total_pixels = image_width * image_height;
      int processed_pixels = 0;

      for(int h = 0; h < image_height; h++)
      {
        p[1] = (h / (double) image_height);
        for(int w = 0; w < image_width; w++)
        {
          p[0] = (w / (double) image_width);
          Vector3 pixel_color(0,0,0);
          for(int i = 0; i < aa; i++)
          { 
            // float r_x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            // float r_y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            // q[0] = w + r_x * (1.0 / image_width);  // Apply horizontal jitter
            // q[1] = h + r_y * (1.0 / image_height); // Apply vertical jitter

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

            pixel_color = pixel_color + ray_color(cur_ray, max_depth, world);

            //Vector3 pixel_color = ray_color(cur_ray);
            
            //Vector3 pixel_color = Vector3(double(w)/(width-1), double(h)/(height-1), 0);
          }

          pixel_color = pixel_color / aa;
          write_color(ofs, pixel_color);
          
          processed_pixels++;
          if (processed_pixels % (total_pixels / 100) == 0) {
              int progress = (processed_pixels * 100) / total_pixels;
              std::cout << "\rProgress: " << progress << "%";
              std::cout.flush();
          }

        }
      }
    }

  private:
    
    int image_height;
    Vector3 position; 
    double pixel_samples_scale;
    Vector3 leftmost_pixel;
    Vector3 pixel_delta_u;
    Vector3 pixel_delta_v;
    Vector3 p, q;
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

  Vector3 ray_color( const Ray& r, int depth, const hittable& world)
  {

    if(depth <= 0)
    {
      return Vector3(0,0,0);
    }

    hit_record rec;
    // interval inter(0.001, infinity);
    // if(world.hit(r, inter, rec))
    // {
    //   Ray scattered;
    //   Vector3 attenuation;

    //   // If the material calculates that the ray should bounce off the certain material and create new ray
    //   if(rec.mat->scatter(r, rec, attenuation, scattered))
    //   {
    //     return multiply(attenuation, ray_color(scattered, depth-1, world));
    //   }

    //   // Vector3 direction = rec.normal + random_unit_vector();
    //   // return 0.5 * ray_color(Ray(rec.p, direction), depth-1, world);
    //   return Vector3(0,0,0);
    // }

    // Vector3 unit_dir = getUnit_Vector(r.getDirection());
    // auto a = 0.5 * (unit_dir.y() + 1.0);
    // return (1.0-a) * Vector3(1.0, 1.0, 1.0) + a*Vector3(0.5, 0.7, 1.0);
    // If the ray hits nothing, return the background color.
    if (!world.hit(r, interval(0.001, infinity), rec))
        return background;

    Ray scattered;
    Vector3 attenuation;
    Vector3 color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat->scatter(r, rec, attenuation, scattered))
        return color_from_emission;

    Vector3 color_from_scatter = multiply(attenuation, ray_color(scattered, depth-1, world));
    // double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);
    // double pdf_value = scattering_pdf;

    // Vector3 color_from_scatter = (scattering_pdf * multiply(attenuation, ray_color(scattered, depth-1, world))) / pdf_value;

    return color_from_emission + color_from_scatter;
    
  }

  Vector3 defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk.
    Vector3 p = random_in_unit_disk();
    return position + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }
    
};

#endif 