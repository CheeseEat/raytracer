#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <vector>
#include "common_vars.h"

using std::make_shared;
using std::shared_ptr;

class Hittable_List : public hittable
{

  public:

    std::vector<shared_ptr<hittable>> objects;

    Hittable_List () {}
    Hittable_List(shared_ptr<hittable> object) {add(object);}

    void clear() {objects.clear();}

    void add(shared_ptr<hittable> object)
    {
      objects.push_back(object);
      bbox = aabb(bbox, object->bounding_box());
    }

    bool hit(const Ray& r, interval ray_t, hit_record& rec) const override 
    {

      hit_record temp_rec;
      bool hit_anything = false;
      auto closest_so_far = ray_t.max;

      for(const auto& object : objects)
      {
        if(object->hit(r, interval(ray_t.min, closest_so_far), temp_rec))
        {
          hit_anything = true;
          closest_so_far = temp_rec.t;
          rec = temp_rec;
        }
      }

      return hit_anything;

    } 

    aabb bounding_box() const override { return bbox; }

    private:
      aabb bbox;

};

#endif