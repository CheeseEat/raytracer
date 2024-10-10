#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "common_vars.h"

class Triangle : public hittable {

  public:
    
    Triangle(Vector3 pointA, Vector3 pointB, Vector3 pointC) : a(pointA), b(pointB), c(pointC), n(getUnit_Vector(cross((b - a), (c - a)))) {}

    bool hit (const Ray& r, interval ray_t, hit_record& rec) const override
    {

      Vector3 n, na, nb, nc;
      Vector3 bary;
      Vector3 d1, d2;
      Vector3 p, q;

      // p and q never intialized 

      //na = (c - b) x (p - b)
      d1 = c - b;
      d2 = q - b;
      na = cross(d1, d2);
      //nb = (a - c) x (p - c)
      d1 = a - c;
      d2 = q - c;
      nb = cross(d1, d2);
      //nc = (b - a) x (p - a)
      d1 = b - a;
      d2 = q - a;
      nc = cross(d1, d2);

      bary[0] = (n * na) / (n * n);
      bary[1] = (n * nb) / (n * n);
      bary[2] = (n * nc) / (n * n);

      // If we remove the test, triangles will tessellate the plane 
      if (bary[0] < 0 || bary[1] < 0 || bary[2] < 0) {
        return false;
      }

      Vector3 temp = bary - r.getOrigin();
      double t = (temp * r.getDirection()) / (r.getDirection() * r.getDirection());

      rec.t = t;
      rec.p = r.eqn(t);
      Vector3 out_norm = cross((b - a), (c - a));
      rec.set_face_normal(r, out_norm);

      std::cout << "thats a hit";

      return true;

    }


  private:

    Vector3 a, b, c;
    Vector3 n;  // Normal

};

#endif


