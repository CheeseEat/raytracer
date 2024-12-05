#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "common_vars.h"

class Triangle : public hittable {

  public:
    
    Triangle(Vector3 pointA, Vector3 pointB, Vector3 pointC) : a(pointA), b(pointB), c(pointC), n(getUnit_Vector(cross((b - a), (c - a)))) {}
    Triangle(Vector3 pointA, Vector3 pointB, Vector3 pointC, const Vector3& uvA, const Vector3& uvB, const Vector3& uvC, Vector3 normalA, Vector3 normalB, Vector3 normalC, shared_ptr<material> mat) : a(pointA), b(pointB), c(pointC), 
                        uv_a(uvA), uv_b(uvB), uv_c(uvC), normal_a(normalA), normal_b(normalB), normal_c(normalC), n(getUnit_Vector(cross((b - a), (c - a)))), mat(mat) {}

    bool hit(const Ray& r, interval ray_t, hit_record& rec) const override {
      
      //std::cout << "hit";

      // Step 1: Calculate the plane of the triangle
      Vector3 edge1 = b - a;
      Vector3 edge2 = c - a;
      Vector3 h = cross(r.getDirection(), edge2);
      double det = edge1 * h;
      Vector3 normal = getUnit_Vector(cross(edge1, edge2));
      //std::cout << "Edge1: " << edge1.getLength() << ", Edge2: " << edge2.getLength() << ", Normal: " << normal.getLength() << "\n";

      // If det is near zero, the ray is parallel to the triangle
      if (std::fabs(det) < 1e-8) {
            //std::cout << "MAMAM";
          return false;
      }
      double inv_det = 1.0 / det;

      // Step 2: Solve for barycentric coordinates
      Vector3 tvec = r.getOrigin() - a;
      double u = (tvec * h) * inv_det;
      if (u < 0.0 || u > 1.0) {
        //std::cout << "d1";
          return false;
      }

      //std::cout << a.x() << " " << a.y() << " " << c.z() << " ";
      //std::cout << r.getOrigin().x() << " " << r.getOrigin().y() << " " << r.getOrigin().z() << " ";

      Vector3 qvec = cross(tvec, edge1);
      double v = (r.getDirection() * qvec) * inv_det;
      if (v < 0.0 || u + v > 1.0) {
        //std::cout << "d2";
          return false;
      }

      // Step 3: Solve for t (intersection distance along ray)
      double t = (edge2 * qvec) * inv_det;
      if (!ray_t.surrounds(t)) {
          //std::cout << "d3";
          return false;
      }

      Vector3 interpolated_normal = (1 - u - v) * normal_a + u * normal_b + v * normal_c;
      interpolated_normal = getUnit_Vector(interpolated_normal);

      std::cout << "Interpolated Normal: "
          << interpolated_normal.x() << ", "
          << interpolated_normal.y() << ", "
          << interpolated_normal.z() << std::endl;

      // Step 4: Fill the hit record
      rec.t = t;
      rec.p = r.eqn(t);  // Intersection point
      rec.set_face_normal(r, interpolated_normal);
      rec.mat = mat;

      rec.u = (1 - u - v) * uv_a.x() + u * uv_b.x() + v * uv_c.x();
      rec.v = (1 - u - v) * uv_a.y() + u * uv_b.y() + v * uv_c.y();

      //std::cout << "Triangle hit\n";
      return true;
    }


    aabb bounding_box() const override {
        Vector3 min_point = min(a, min(b, c));
        Vector3 max_point = max(a, max(b, c));
        return aabb(min_point, max_point);
    }

  private:

    Vector3 a, b, c;
    Vector3 uv_a, uv_b, uv_c; 
    Vector3 normal_a, normal_b, normal_c;
    Vector3 n;  // Normal
    shared_ptr<material> mat;
    aabb bbox;

};

#endif
