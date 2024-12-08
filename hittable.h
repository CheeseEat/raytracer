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

class rotate_y : public hittable {
  public:

    bool hit(const Ray& r, interval ray_t, hit_record& rec) const override {

        // Transform the ray from world space to object space.

        auto origin = Vector3(
            (cos_theta * r.getOrigin().x()) - (sin_theta * r.getOrigin().z()), r.getOrigin().y(), (sin_theta * r.getOrigin().x()) + (cos_theta * r.getOrigin().z())
        );

        auto direction = Vector3(
            (cos_theta * r.getDirection().x()) - (sin_theta * r.getDirection().z()), r.getDirection().y(), (sin_theta * r.getDirection().x()) + (cos_theta * r.getDirection().z())
        );

        Ray rotated_r(origin, direction, r.getTime());

        // Determine whether an intersection exists in object space (and if so, where).

        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        // Transform the intersection from object space back to world space.

        rec.p = Vector3(
            (cos_theta * rec.p.x()) + (sin_theta * rec.p.z()), rec.p.y(), (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
        );

        rec.normal = Vector3(
            (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()), rec.normal.y(), (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
        );

        return true;
    }

    rotate_y(shared_ptr<hittable> object, double angle) : object(object) {
        auto radians = degrees_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        Vector3 min( infinity,  infinity,  infinity);
        Vector3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    Vector3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    aabb bounding_box() const override { return bbox; }

  private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;

};

 #endif