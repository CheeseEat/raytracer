#ifndef RAY_H
#define RAY_H

#include "vector3.h"

// A ray is defined as ray = Ad + o, o is ray origin and d is normailized direction

class Ray {
  public:
    
    Ray() {}
    Ray(const Vector3& origin_param, const Vector3& direction_param) : origin(origin_param), direction(direction_param) {}
    Ray(const Vector3& origin_param, const Vector3& direction_param, double time_param) : origin(origin_param), direction(direction_param), time(time_param) {}

    const Vector3& getOrigin()      const { return origin; }
    const Vector3& getDirection()   const { return direction; }
    double getTime() const { return time; }

    Vector3 eqn(double t) const
    {
      return origin + t * direction;
    }

  private: 
    Vector3 origin;
    Vector3 direction;
    double time;
    
};

#endif 
