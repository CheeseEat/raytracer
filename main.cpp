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
#include "quad.h"
#include "constant_medium.h"

double dot(const Vector3& v1, const Vector3& v2)
{
  return ((v1.x() * v2.x()) + (v1.y() * v2.y()) + (v1.z() * v2.z()));
}

void final_scene() {

    //Output file for the rendered image
    std::ofstream ofs("output.ppm");
    if (!ofs) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    // World objects
    Hittable_List world;

    auto checker = make_shared<checkered>(0.32, Vector3(0.2, 0.3, 0.1), Vector3(0.9, 0.9, 0.9));
    world.add(make_shared<Sphere>(Vector3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    // Reflective metal sphere in the center
    auto metal_material = make_shared<metal>(Vector3(0.8, 0.6, 0.2), 0.01);
    world.add(make_shared<Sphere>(Vector3(0, 1, 0), 1.0, metal_material));

    // Glass sphere
    auto glass_material = make_shared<dielectric>(1.5); // Refractive index of glass
    world.add(make_shared<Sphere>(Vector3(1, 1, -2), 1.0, glass_material)); // Glass sphere

    auto difflight = make_shared<diffuse_light>(Vector3(4,4,4));
    world.add(make_shared<quad>(
      Vector3(-1, 2, 2),    // Raised position
      Vector3(2, 0, 0),      // Width vector
      Vector3(0, -1, 1),     // Adjust height vector to tilt toward objects, 
      difflight)); 

    Vector3 pointA(-1, 0, -1);
    Vector3 pointB(1, 0, -1);
    Vector3 pointC(0, 2, -1);

    Vector3 uvA(0, 0, 0); // UV coordinates
    Vector3 uvB(1, 0, 0);
    Vector3 uvC(0.5, 1, 0);

    Vector3 normalA(0, 0, 1); // Normals for smooth shading
    Vector3 normalB(0, 0, 1);
    Vector3 normalC(0, 0, 1);

    // Smaller spheres with Lambertian materials
    auto orange = make_shared<image_texture>("orange.jpg");
    auto lambertian_material = make_shared<lambertian>(orange);
    for (int i = -5; i <= 5; i += 2) {
        for (int j = -5; j <= 5; j += 2) {
            Vector3 center(i + 0.9 * random_double(), 0.2, j + 0.9 * random_double());
            double radius = 0.3;
            world.add(make_shared<Sphere>(Vector3(i, radius, j), center, radius, lambertian_material));
        }
    }

    std::vector<Vector3> vertices;
    std::vector<Vector3> uv_coords;
    std::vector<int> indices;
    std::vector<Vector3> normals;

    if (!load_obj("funny.obj", vertices, indices, uv_coords, normals)) {
        std::cerr << "Failed to load OBJ file." << std::endl;
        return;
    }

    std::cout << "Loaded OBJ file successfully!" << std::endl;
    std::cout << "Vertices: " << vertices.size() << ", Indices: " << indices.size() << std::endl;

    // Adding trinangle mesh, applying perline noise, rotating and translating
    auto pertext = make_shared<noise_texture>(4);
    auto text_mesh  = make_shared<solid_color>(1.0, 0.1, 0.7);
    auto mesh_mat_perlin   = make_shared<lambertian>(pertext);
    auto mesh = make_shared<TriangleMesh>(vertices, uv_coords, indices, normals, mesh_mat_perlin);
    auto translation = make_shared<rotate_y>(mesh, -30);
    auto translation2 = make_shared<translate>(mesh, Vector3(2,0,0));
    world.add(translation2);
    
    cube_map_texture cube_map(
        "posx.jpg", "negx.jpg", // Positive/Negative X
        "posy.jpg", "negy.jpg", // Positive/Negative Y
        "posz.jpg", "negz.jpg"  // Positive/Negative Z
    );

    // Camera setup
    int aa = 400; // Anti-aliasing samples
    Camera cam(ofs, aa, cube_map);

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1280;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = Vector3(0.0, 0.0, 0.0);

    cam.vfov     = 50;
    cam.lookfrom = Vector3(3, 2, 4);
    cam.lookat   = Vector3(0, 1, 0);
    cam.vup      = Vector3(0, 1, 0);

    cam.defocus_angle = 0.7; // Depth of field
    cam.focus_dist = (cam.lookfrom - cam.lookat).getLength();

    // Render the scene
    cam.render(world, 15);

    ofs.close();

}

int main()
{
  
  final_scene();

  std::cout << "done";

  return 0;

}