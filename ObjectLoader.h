#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>
#include "Vector3.h"
#include "external/tiny_obj_loader.h"
#include "Vector2.h"

bool load_obj(const std::string& filename, std::vector<Vector3>& vertices, std::vector<int>& indices, std::vector<Vector3>& uv_coords, std::vector<Vector3>& normals) {
    tinyobj::attrib_t attrib; std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
        std::cerr << warn << err << std::endl;
        return false;
    }

    // Extract vertices
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        vertices.emplace_back(
            attrib.vertices[i + 0],
            attrib.vertices[i + 1],
            attrib.vertices[i + 2]
        );
    }

    if (!attrib.normals.empty()) {
      for (size_t i = 0; i < attrib.normals.size(); i += 3) {
          normals.emplace_back(
              attrib.normals[i + 0],
              attrib.normals[i + 1],
              attrib.normals[i + 2]
          );
      }
    }

    //Extract indices and UV coordinates
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            indices.push_back(index.vertex_index);

            if (index.texcoord_index >= 0) {
                uv_coords.emplace_back(
                    attrib.texcoords[2 * index.texcoord_index],
                    attrib.texcoords[2 * index.texcoord_index + 1],
                    0.0
                    //attrib.texcoords[2 * index.texcoord_index + 2]
                );
            }
        }
    }

    if (normals.empty()) {
        normals.resize(vertices.size(), Vector3(0, 0, 0));

        for (const auto& shape : shapes) {
            for (size_t i = 0; i < shape.mesh.indices.size(); i += 3) {
                int idx0 = shape.mesh.indices[i + 0].vertex_index;
                int idx1 = shape.mesh.indices[i + 1].vertex_index;
                int idx2 = shape.mesh.indices[i + 2].vertex_index;

                Vector3 v0 = vertices[idx0];
                Vector3 v1 = vertices[idx1];
                Vector3 v2 = vertices[idx2];

                // Compute the face normal
                Vector3 edge1 = v1 - v0;
                Vector3 edge2 = v2 - v0;
                Vector3 face_normal = getUnit_Vector(cross(edge1, edge2));

                // Add the face normal to each vertex normal
                normals[idx0] = normals[idx0] + face_normal;
                normals[idx1] = normals[idx1] + face_normal;
                normals[idx2] = normals[idx2] + face_normal;
            }
        }

        // Normalize all vertex normals
        for (auto& normal : normals) {
            normal = getUnit_Vector(normal);
        }
        
    }

    return true;

}

#endif