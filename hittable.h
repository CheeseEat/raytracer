 #ifndef HITTABLE_H
 #define HITTABLE_H

#include "common_vars.h"
#include "Vector3.h"
#include "Ray.h"
#include "interval.h"

class material;

class hit_record
{
  public:
    Vector3 p;
    Vector3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face;

    void set_face_normal(const Ray& r, const Vector3& outward_normal)
    {
      front_face = r.getDirection() * outward_normal;
      normal = front_face ? outward_normal : -outward_normal;
    }

};

class hittable
{
  public:
    virtual ~hittable() = default;

    virtual bool hit(const Ray& r, interval ray_t, hit_record& rec) const = 0;
    
};

 #endif