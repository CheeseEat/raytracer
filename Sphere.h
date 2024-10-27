#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "Vector3.h"

class Sphere : public hittable {

  public:

    // Moving Sphere
    Sphere(const Vector3& center1, const Vector3& center2, double radius, shared_ptr<material> mat) : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat(mat) 
    {
      Vector3 rvec = Vector3(radius, radius, radius);
      aabb box1(center.eqn(0) - rvec, center.eqn(0) + rvec);
      aabb box2(center.eqn(1) - rvec, center.eqn(1) + rvec);
    }

    // Stationary Sphere
    Sphere(const Vector3& origin, double radius, shared_ptr<material> mat) : center(origin, Vector3(0,0,0)), radius(std::fmax(0, radius)), mat(mat) 
    {
      Vector3 rvec = Vector3(radius, radius, radius);
      bbox = aabb(origin - rvec, origin + rvec);
    }
 
    bool hit (const Ray& r, interval ray_t, hit_record& rec) const override
    {
      
      Vector3 curCenter = center.eqn(r.getTime());
      Vector3 oc = curCenter - r.getOrigin();
      
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
      double root = (h - sqrt_temp) / a;
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
      Vector3 out_norm = (rec.p - curCenter) / radius;
      rec.set_face_normal(r, out_norm);
      rec.mat = mat;

      return true;

    }

    aabb bounding_box() const override { return bbox; }
  
  private:

    //Vector3 origin;
    Ray center; // For motion
    double radius;
    shared_ptr<material> mat;
    aabb bbox;

};

#endif