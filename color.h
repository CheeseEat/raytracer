#ifndef COLOR_H
#define COLOR_H

#include "common_vars.h"
#include "Vector3.h"
#include <iostream>

inline double gamma(double linear)
{
  if(linear > 0)
  {
    return std::sqrt(linear);
  }
  return 0;
}

inline void write_color(std::ostream& out, const Vector3& pixel_color)
{
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    r = gamma(r);
    g = gamma(b);
    b = gamma(b);

    // Clamp and convert to int
    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif 
