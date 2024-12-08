 #ifndef HITTABLE_H
 #define HITTABLE_H

#include "common_vars.h"
#include "Vector3.h"
#include "Ray.h"
#include "interval.h"
#include "aabb.h"

class material;

class hit_record
{
  public:
    Vector3 p;
    Vector3 normal;
    shared_ptr<material> mat;
    double t;
    double u, v;          // Texture coordinates
    bool front_face;

    void set_face_normal(const Ray& r, const Vector3& outward_normal)
    {
      front_face = r.getDirection() * outward_normal < 0;
      normal = front_face ? outward_normal : -outward_normal;
    }

};

class hittable
{
  public:
    virtual ~hittable() = default;

    virtual bool hit(const Ray& r, interval ray_t, hit_record& rec) const = 0;
    
    virtual aabb bounding_box() const = 0;
    
};

class translate : public hittable 
{
    public:

    translate(shared_ptr<hittable> object, const Vector3& offset): object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const Ray& r, interval ray_t, hit_record& rec) const override {
        // Move the ray backwards by the offset
        Ray offset_r(r.getOrigin() - offset, r.getDirection(), r.getTime());

        // Determine whether an intersection exists along the offset ray (and if so, where)
        if (!object->hit(offset_r, ray_t, rec))
            return false;

        // Move the intersection point forwards by the offset
        rec.p = rec.p + offset;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

  private:
    shared_ptr<hittable> object;
    Vector3 offset;
    aabb bbox;
};

 #endif