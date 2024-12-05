#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>
#include "Vector3.h"
#include "external/tiny_obj_loader.h"
#include "Vector2.h"

bool load_obj(const std::string& filename, std::vector<Vector3>& vertices, std::vector<int>& indices, std::vector<Vector3>& uv_coords) {
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

    for (size_t i = 0; i < attrib.normals.size(); i += 3) {
      vertex_normals.emplace_back(
          attrib.normals[i + 0],
          attrib.normals[i + 1],
          attrib.normals[i + 2]
      );
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

    return true;

}

#endif