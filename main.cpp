#include "common_vars.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hittable.h"
#include "Hittable_List.h"

#include "Camera.h"
#include "Vector3.h"
#include "interval.h"
#include "Color.h"
#include "Sphere.h"
#include "Triangle.h"

double dot(const Vector3& v1, const Vector3& v2)
{
  return ((v1.x() * v2.x()) + (v1.y() * v2.y()) + (v1.z() * v2.z()));
}

int main()
{

  std::ofstream ofs("output.ppm");
  if (!ofs) {
      std::cerr << "Error opening file for writing!" << std::endl;
      return -1;
  }

  std::cout << "correct program" << std::endl;

  int width = 1080;
  //int height = 600;
  float aspect_ratio = 16.0/9.0;
  int aa = 100;

  Hittable_List world;

  auto material_right  = make_shared<metal>(Vector3(0.8, 0.6, 0.2));
  auto material_center = make_shared<lambertian>(Vector3(0.1, 0.2, 0.5));
  auto material_left   = make_shared<metal>(Vector3(0.8, 0.8, 0.8));

  world.add(make_shared<Sphere>(Vector3(0,0,-1), 0.5, material_left));
  world.add(make_shared<Sphere>(Vector3(0,1,-1), 0.5, material_left));
  world.add(make_shared<Sphere>(Vector3(0,-100.5,-1), 100, material_right));
  //world.add(make_shared<Triangle>(Vector3(0.3, 0.3, 0), Vector3(0.6, 0.4, 0), Vector3(0.5, 0.7, 0)));

  //format is r g b \n
  Camera cam (ofs, aa);
  cam.image_width = width;
  cam.aspect_ratio = aspect_ratio;
  cam.render(world);

  ofs.close();
  std::cout << "done";

  return 0;

}