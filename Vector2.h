#ifndef VECTOR2_H
#define VECTOR2_H

#include "common_vars.h"
#include <cmath>

class Vector2 {
  public:
    
    Vector2() : values{0, 0} {}
    Vector2(double x, double y) : values{x, y} {}

    double x() const { return values[0]; }
    double y() const { return values[1]; }

  private:
    
    // The values of the vector
    double values[2];
};

#endif 