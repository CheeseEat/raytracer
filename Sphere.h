#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "Vector3.h"

class Sphere : public hittable {

  public:
    Sphere(const Vector3& origin, double radius) : origin(origin), radius(std::fmax(0, radius))
    {

    }

    bool hit (const Ray& r, interval ray_t, hit_record& rec) const override
    {
      
      Vector3 oc = origin - r.getOrigin();
      
      double a = std::pow(r.getDirection().getLength(), 2);
      double h = r.getDirection() * oc;
      double c = std::pow(oc.getLength(), 2) - std::pow(radius, 2); 
      double discriminant = h*h - a*c;

      // if 0 then 1 root, if positive then 2 roots, else negative means 0 roots
      if(discriminant < 0)
      {
        return false;
      }
      
      // Checking to see if roots are in acceptable range specified by tmin and tmax
      double sqrt_temp = std::sqrt(discriminant);
      double root = (h - sqrt_temp);
      if(!ray_t.surrounds(root))
      {
        root = (h + sqrt_temp) / a;
        if(!ray_t.surrounds(root));
        {
          return false;   // Neither of the roots are in acceptable range
        }
      }

      // Setting where the ray hit the point in hit record
      rec.t = root;
      rec.p = r.eqn(rec.t);
      Vector3 out_norm = (rec.p - origin) / radius;
      rec.set_face_normal(r, out_norm);
      rec.mat = mat;

      return true;

    }
  
  private:

    Vector3 origin;
    double radius;
    shared_ptr<material> mat;

};

#endif