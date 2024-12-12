#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "hittable.h"
#include "Vector3.h"
#include "Vector2.h"
#include <vector>
#include "Triangle.h"
#include <cfloat>

class TriangleMesh : public hittable {
  public:
    TriangleMesh(const std::vector<Vector3>& vertices,
                 const std::vector<Vector3>& uv_coords,
                 const std::vector<int>& indices,
                 const std::vector<Vector3>& normals,
                 shared_ptr<material> mat)
        : vertices(vertices), uv_coords(uv_coords), indices(indices), normals(normals), mat(mat) {std::cout << "let there be light";}

    bool hit(const Ray& r, interval ray_t, hit_record& rec) const override {
        bool hit_anything = false;
        hit_record temp_rec;
        //std::cout << indices.size();
        for (size_t i = 0; i < indices.size(); i += 3) {
            Vector3 a = vertices[indices[i]];
            Vector3 b = vertices[indices[i + 1]];
            Vector3 c = vertices[indices[i + 2]];

           // Vertex normals
            Vector3 normal_a = normals.empty() ? Vector3(0, 0, 0) : normals[indices[i]];
            Vector3 normal_b = normals.empty() ? Vector3(0, 0, 0) : normals[indices[i + 1]];
            Vector3 normal_c = normals.empty() ? Vector3(0, 0, 0) : normals[indices[i + 2]];

            // UV coordinates (fallback to zero if missing)
            Vector3 uv_a = uv_coords.empty() ? Vector3(0, 0, 0) : uv_coords[indices[i]];
            Vector3 uv_b = uv_coords.empty() ? Vector3(0, 0, 0) : uv_coords[indices[i + 1]];
            Vector3 uv_c = uv_coords.empty() ? Vector3(0, 0, 0) : uv_coords[indices[i + 2]];

            Triangle triangle(a, b, c, uv_a, uv_b, uv_c, normal_a, normal_b, normal_c, mat);
            if (triangle.hit(r, ray_t, temp_rec)) {
                hit_anything = true;
                ray_t = interval(ray_t.min, temp_rec.t); // Update the interval
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    aabb bounding_box() const override {
      if (vertices.empty() || indices.empty()) {
        return aabb(); // Return an empty box
      }

      Vector3 min_point(FLT_MAX, FLT_MAX, FLT_MAX);
      Vector3 max_point(-FLT_MAX, -FLT_MAX, -FLT_MAX);

      // Iterate through all triangles
      for (size_t i = 0; i < indices.size(); i += 3) {
          Vector3 a = vertices[indices[i]];
          Vector3 b = vertices[indices[i + 1]];
          Vector3 c = vertices[indices[i + 2]];

          // Compute per-triangle bounding box
          Vector3 tri_min(
              std::fmin(a.x(), std::fmin(b.x(), c.x())),
              std::fmin(a.y(), std::fmin(b.y(), c.y())),
              std::fmin(a.z(), std::fmin(b.z(), c.z()))
          );

          Vector3 tri_max(
              std::fmax(a.x(), std::fmax(b.x(), c.x())),
              std::fmax(a.y(), std::fmax(b.y(), c.y())),
              std::fmax(a.z(), std::fmax(b.z(), c.z()))
          );

          // Expand mesh bounding box
          min_point = min(min_point, tri_min);
          max_point = max(max_point, tri_max);

      }
      return aabb(min_point, max_point);
    }

  private:
    std::vector<Vector3> vertices;
    std::vector<Vector3> uv_coords;
    std::vector<int> indices;
    std::vector<Vector3> normals;
    shared_ptr<material> mat;
};

#endif