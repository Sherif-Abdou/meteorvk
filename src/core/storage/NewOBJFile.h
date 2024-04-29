//
// Created by Sherif Abdou on 4/29/24.
//

#ifndef VULKAN_ENGINE_NEWOBJFILE_H
#define VULKAN_ENGINE_NEWOBJFILE_H

#include <glm/glm.hpp>
#include <boost/container_hash/hash.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "Vertex.h"

class NewOBJFile {
public:
    struct Face {
        std::vector<uint32_t> vertexIndices {};
        std::vector<uint32_t> normalIndices {};
        std::vector<uint32_t> texCoordIndices {};

        uint32_t material_id = -1;
    };

    struct Point {
        uint32_t vertexIndex {};
        uint32_t normalIndex {};
        uint32_t texCoordIndex {};
        uint32_t material_id = -1;

        bool operator==(const Point& other) {
            return vertexIndex == other.vertexIndex
                && normalIndex == other.normalIndex
                && texCoordIndex == other.texCoordIndex
                && material_id == other.material_id;
        }
    };
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<Face> faces;

    std::vector<std::string> mtl_paths;
    std::unordered_map<std::string, uint32_t> mtl_name_map;
    std::unordered_map<uint32_t, std::string> mtl_index_map;

public:
    explicit NewOBJFile(std::ifstream& in);

    glm::vec3 calculateTangent(const Face &face, int offset);

    struct IndexPair {
        std::vector<unsigned int> indices;
        std::vector<Vertex> vertices;
    };

    IndexPair exportIndexedBuffer();

    static NewOBJFile fromFilePath(const std::string & file_path);

    const std::vector<std::string>& getMaterialPaths() const;

    const std::unordered_map<uint32_t, std::string>& getMTLIndexMap() const;
};


#endif //VULKAN_ENGINE_NEWOBJFILE_H
