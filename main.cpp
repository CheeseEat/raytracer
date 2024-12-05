#include "common_vars.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hittable.h"
#include "Hittable_List.h"

#include "bvh_node.h"
#include "Camera.h"
#include "Vector3.h"
#include "interval.h"
#include "Color.h"
#include "Sphere.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "ObjectLoader.h"

double dot(const Vector3& v1, const Vector3& v2)
{
  return ((v1.x() * v2.x()) + (v1.y() * v2.y()) + (v1.z() * v2.z()));
}

#define TX 1300
#define TY 827
//int8_t img[Y][X][3]; // Zero by placement
// Vector3 texture[TY][TX];
// void load_texture()
// {
//   FILE *f;
//   char s[80];
//   int x, y;
//   int r, g, b;
  
//   f = fopen("earth2.ppm", "r"); // Text file.  Don't open in binary mode.
//   fscanf(f, "%s\n%d %d %d\n", s, &x, &x, &x); // Read header
//   for (y = 0; y < TY; y++) {
//     for (x = 0; x < TX; x++) {
//       fscanf(f, "%d %d %d ", &r, &g, &b);
//       texture[y][x][0] = r / 256.0;
//       texture[y][x][1] = g / 256.0;
//       texture[y][x][2] = b / 256.0;
//     }
//   }
// }

void bouncing_sphere()
{
  // auto material_ground = make_shared<lambertian>(Vector3(0.8, 0.8, 0.0));
  // auto material_right  = make_shared<metal>(Vector3(0.8, 0.6, 0.2), 0.8);
  // auto material_center = make_shared<lambertian>(Vector3(0.1, 0.2, 0.5));
  // auto material_left   = make_shared<dielectric>(1.50);
  // auto material_bubble = make_shared<dielectric>(1.00 / 1.50);

  // world.add(make_shared<Sphere>(Vector3( 0.0, -100.5, -1.0), 100.0, material_ground));
  // world.add(make_shared<Sphere>(Vector3( 0.0,    0.0, -1.2),   0.5, material_center));
  // world.add(make_shared<Sphere>(Vector3(-1.0,    0.0, -1.0),   0.5, material_left));
  // world.add(make_shared<Sphere>(Vector3(-1.0,    0.0, -1.0),   0.4, material_bubble));
  // world.add(make_shared<Sphere>(Vector3( 1.0,    0.0, -1.0),   0.5, material_right));

  // Vector3 center = Vector3(0,0,0);
  // auto center2 = center + Vector3(0, 0.2, 0.2);
  // world.add(make_shared<Sphere>(center, center2, 0.2, material_right));

  std::ofstream ofs("output.ppm");
  if (!ofs) {
      std::cerr << "Error opening file for writing!" << std::endl;
      //return -1;
  }

  std::cout << "correct program" << std::endl;

  int width = 400;
  //int height = 600;
  float aspect_ratio = 16.0/9.0;
  int aa = 30;

  Hittable_List world;
  auto checker = make_shared<checkered>(0.32, Vector3(.2, .3, .1), Vector3(.9, .9, .9));
  world.add(make_shared<Sphere>(Vector3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

  for (int a = -11; a < 11; a++) {
      for (int b = -11; b < 11; b++) {
          auto choose_mat = random_double();
          Vector3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

          if ((center - Vector3(4, 0.2, 0)).getLength() > 0.9) {
              shared_ptr<material> sphere_material;

              if (choose_mat < 0.8) {
                  // diffuse
                  auto albedo = multiply(Vector3::random(), Vector3::random());
                  sphere_material = make_shared<lambertian>(albedo);
                  auto center2 = center + Vector3(0, 1, 1);
                  //world.add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
                  world.add(make_shared<Sphere>(center, 0.2, sphere_material));
              } else if (choose_mat < 0.95) {
                  // metal
                  auto albedo = Vector3::random(0.5, 1);
                  auto fuzz = random_double(0, 0.5);
                  sphere_material = make_shared<metal>(albedo, fuzz);
                  world.add(make_shared<Sphere>(center, 0.2, sphere_material));
              } else {
                  // glass
                  sphere_material = make_shared<dielectric>(1.5);
                  world.add(make_shared<Sphere>(center, 0.2, sphere_material));
              }
          }
      }
  }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<Sphere>(Vector3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(Vector3(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Vector3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(Vector3(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(Vector3(4, 1, 0), 1.0, material3));

    world = Hittable_List(make_shared<bvh_node>(world));
  
  // auto material_ground = make_shared<lambertian>(Vector3(0.8, 0.8, 0.0));
  // auto material_center = make_shared<lambertian>(Vector3(0.1, 0.2, 0.5));
  // auto material_left   = make_shared<dielectric>(1.50);
  // auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
  // auto material_right  = make_shared<metal>(Vector3(0.8, 0.6, 0.2), 1.0);

  // world.add(make_shared<Sphere>(Vector3( 0.0, -100.5, -1.0), 100.0, material_ground));
  // world.add(make_shared<Sphere>(Vector3( 0.0,    0.0, -1.2),   0.5, material_center));
  // world.add(make_shared<Sphere>(Vector3(-1.0,    0.0, -1.0),   0.5, material_left));
  // world.add(make_shared<Sphere>(Vector3(-1.0,    0.0, -1.0),   0.4, material_bubble));
  // world.add(make_shared<Sphere>(Vector3( 1.0,    0.0, -1.0),   0.5, material_right));

  //world.add(make_shared<Triangle>(Vector3(0.3, 0.3, 0), Vector3(0.6, 0.4, 0), Vector3(0.5, 0.7, 0)));

  //format is r g b \n
  Camera cam (ofs, aa);
  cam.image_width = width;
  cam.aspect_ratio = aspect_ratio;
  cam.max_depth = 50;

  cam.vfov     = 20;
  
  cam.lookfrom = Vector3(13,2,3);
  cam.lookat   = Vector3(0,0,0);
  cam.vup      = Vector3(0,1,0);
  
  //cam.lookfrom = Vector3(0,0,1);
  //cam.lookat   = Vector3(0,0,-1);
  //cam.vup      = Vector3(0,1,0);

  //cam.defocus_angle = 0.75;
  //cam.focus_dist    = 3.4;

  ofs.close();

}

void checkered_sphere()
{

  std::ofstream ofs("output.ppm");
  if (!ofs) {
      std::cerr << "Error opening file for writing!" << std::endl;
      //return -1;
  }

  Hittable_List world;

  std::cout << "correct program" << std::endl;

  int width = 400;
  //int height = 600;
  float aspect_ratio = 16.0/9.0;
  int aa = 30;

  auto checker = make_shared<checkered>(0.32, Vector3(.2, .3, .1), Vector3(.9, .9, .9));

  world.add(make_shared<Sphere>(Vector3(0,-10, 0), 10, make_shared<lambertian>(checker)));
  world.add(make_shared<Sphere>(Vector3(0, 10, 0), 10, make_shared<lambertian>(checker)));

  Camera cam(ofs, aa);

  cam.aspect_ratio      = 16.0 / 9.0;
  cam.image_width       = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth         = 50;

  cam.vfov     = 20;
  cam.lookfrom = Vector3(13,2,3);
  cam.lookat   = Vector3(0,0,0);
  cam.vup      = Vector3(0,1,0);

  cam.defocus_angle = 0;

  cam.render(world);

  ofs.close();

}

Vector3 rotate(const Vector3& v, const Vector3& axis, double angle) {
    double cos_theta = std::cos(angle);
    double sin_theta = std::sin(angle);

    // Rodrigues' rotation formula
    return v * cos_theta +
           cross(axis, v) * sin_theta +
           axis * (axis * v) * (1 - cos_theta);
}

void rotate_cube(std::vector<Vector3>& vertices, const Vector3& axis, double angle) {
    for (auto& vertex : vertices) {
        vertex = rotate(vertex, axis, angle);
    }
}

void yoSoy() {

  std::ofstream ofs("output.ppm");
  if (!ofs) {
      std::cerr << "Error opening file for writing!" << std::endl;
      //return -1;
  }

  Hittable_List thing;

  auto texture = make_shared<image_texture>("yoSoy.jpg");
  auto surface = make_shared<lambertian>(texture);
  auto text1  = make_shared<solid_color>(255, 255, 255);
  //auto surface2 = make_shared<lambertian>(text1);
  auto globe = make_shared<Sphere>(Vector3(0,0,0), 2, surface);
  //auto tri  = make_shared<Triangle>(a, b, c, surface2);
  
  int aa = 30;
  Camera cam(ofs, aa);

  cam.aspect_ratio      = 16.0 / 9.0;
  cam.image_width       = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth         = 50;

  cam.vfov     = 20;
  cam.lookfrom = Vector3(0,0,12);
  cam.lookat   = Vector3(0,0,0);
  cam.vup      = Vector3(0,1,0);

  cam.defocus_angle = 0;

  //thing.add(globe);
  // thing.add(make_shared<Triangle>(
  //   Vector3(0.3, 0.3, 0),
  //   Vector3(0.6, 0.4, 0),
  //   Vector3(0.5, 0.7, 0),
  //   make_shared<lambertian>(text1)));

  auto texture2 = make_shared<image_texture>("leon.jpg");
  auto material_right  = make_shared<metal>(Vector3(0.8, 0.6, 0.2), 0.2);
  auto material2 = make_shared<lambertian>(texture2);

  Vector3 a(0, 0, 0); // Triangle vertices
  Vector3 b(1, 0, 0);
  Vector3 c(0, 1, 0);

  Vector3 uv_a(0, 0, 0); // UV coordinates
  Vector3 uv_b(1, 1, 0);
  Vector3 uv_c(0, 1, 1);

  //thing.add(make_shared<Triangle>(a, b, c, uv_a, uv_b, uv_c, material2));

  // std::vector<Vector3> vertices = {
  //   Vector3(0, 0, 0),
  //   Vector3(1, 0, 0),
  //   Vector3(0, 1, 0),
  //   Vector3(1, 1, 0)
  // };

  // std::vector<Vector3> uv_coords = {
  //     Vector3(0, 0, 0),
  //     Vector3(1, 0, 0),
  //     Vector3(0, 1, 0),
  //     Vector3(1, 1, 0)
  // };

  // std::vector<int> indices = {
  //     0, 1, 2, // First triangle
  //     1, 3, 2  // Second triangle
  // };
 
  std::vector<Vector3> vertices;
  std::vector<Vector3> uv_coords;
  std::vector<int> indices;
  std::vector<Vector3> normals;

  if (!load_obj("test.obj", vertices, indices, uv_coords, normals)) {
      std::cerr << "Failed to load OBJ file." << std::endl;
      return;
  }

  //rotate_cube(vertices, Vector3(0, 1, 0), degrees_radians(45));

  std::cout << "Loaded OBJ file successfully!" << std::endl;
  std::cout << "Vertices: " << vertices.size() << ", Indices: " << indices.size() << std::endl;

  auto material = make_shared<lambertian>(texture2);
  auto metal_material = make_shared<metal>(Vector3(0.8, 0.8, 0.8), 0.01); // Shiny metal
  auto checker = make_shared<checkered>(0.32, Vector3(.2, .3, .1), Vector3(.9, .9, .9));
  auto mesh = make_shared<TriangleMesh>(vertices, uv_coords, indices, normals, metal_material);
   auto pertext = make_shared<noise_texture>(4);

  thing.add(make_shared<Sphere>(Vector3( 0.0, -100.5, -1.0), 100.0, make_shared<lambertian>(checker)));
  thing.add(make_shared<Sphere>(Vector3( 0.0, 0.0, 0.0), 1.0, make_shared<lambertian>(pertext)));
  //thing.add(mesh);

  //thing.add(make_shared<Sphere>(Vector3( 0.0, 0, 0), 1.0, metal_material));

  //cam.render(Hittable_List(globe));
  cam.render(thing);

}


int main()
{
  
  yoSoy();

  std::cout << "done";

  return 0;

}