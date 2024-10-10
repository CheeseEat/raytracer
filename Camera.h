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

    const Vector3& getPosition()    const { return position; }
    double getViewport_width()      const { return image_width; }
    double getViewport_height()     const { return image_height; }
    double getAspect_ratio()        const { return aspect_ratio; }

    Camera(std::ofstream& ofs, int aa) : ofs(ofs), aa(aa) {}

    void render(const hittable& world)
    {

      initialize();

      for(int h = 0; h < image_height; h++)
      {
        p[1] = (h / (double) image_height);
        for(int w = 0; w < image_width; w++)
        {
          p[0] = (w / (double) image_width);
          Vector3 pixel_color(0,0,0);
          for(int i = 0; i < aa; i++)
          { 
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            q[1] = p[1] + r * (1.0 / (h + 1e-6)); 
            q[0] = p[0] + r * (1.0 / (w + 1e-6));

            //Send out raysence between the camera's position and the targeted pixels location
            Vector3 cur_pixel_location = leftmost_pixel + Vector3(w * pixel_delta_u.x(), h * pixel_delta_v.y(), 0);

            Vector3 ray_direction =  cur_pixel_location - position;
            Ray cur_ray(position, ray_direction);

            pixel_color = pixel_color + ray_color(cur_ray, max_depth, world);

            //Vector3 pixel_color = ray_color(cur_ray);
            
            //Vector3 pixel_color = Vector3(double(w)/(width-1), double(h)/(height-1), 0);
          }

          pixel_color = pixel_color / aa;
          write_color(ofs, pixel_color);

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
    std::ofstream& ofs;
    int aa;                       // Antialisaing value

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    ofs << "P3\n"; // P3 for colors in ASCII
    ofs << image_width << " " << image_height << "\n"; // Determine image size
    ofs << "255\n"; // Max color value

    position = Vector3(0, 0, 0);
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(image_width)/image_height);

    pixel_samples_scale = 1.0 / samples_per_pixel;

    Vector3 viewport_u(viewport_width, 0 , 0);   
    Vector3 viewport_v(0, -viewport_height, 0);

    pixel_delta_u = viewport_u / (image_width * 1.0);
    pixel_delta_v = viewport_v / (image_height * 1.0);

    Vector3 viewport_up_leftside = position - Vector3(0, 0, focal_length) - (viewport_u / 2) - (viewport_v / 2);
    leftmost_pixel = viewport_up_leftside + Vector3(pixel_delta_u.x() * 0.5, pixel_delta_v.y() * 0.5, 0);
  
  }

  Vector3 ray_color( const Ray& r, int depth, const hittable& world)
  {

    if(depth <= 0)
    {
      return Vector3(0,0,0);
    }

    hit_record rec;
    interval inter(0.001, infinity);
    if(world.hit(r, inter, rec))
    {
      Ray scattered;
      Vector3 attenuation;
      if(rec.mat->scatter(r, rec, attenuation, scattered))
      {
        return multiply(attenuation, ray_color(scattered, depth-1, world));
      }

      Vector3 direction = rec.normal + random_unit_vector();
      return 0.5 * ray_color(Ray(rec.p, direction), depth-1, world);
    }

    Vector3 unit_dir = getUnit_Vector(r.getDirection());
    auto a = 0.5 * (unit_dir.y() + 1.0);
    return (1.0-a) * Vector3(1.0, 1.0, 1.0) + a*Vector3(0.5, 0.7, 1.0);
    
  }
    
};

#endif 