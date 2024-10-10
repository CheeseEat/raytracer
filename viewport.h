#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "vector3.h"

class viewport {
  public:
    
    viewport() {}
    viewport(const Vector3& position_param, double focal_length_param) : position(position_param), focal_length(focal_length_param) {}

  private:
    
    Vector3 position;
    double focal_length;
    
};

#endif 