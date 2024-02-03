//
// Created by sheri on 6/20/2023.
//

#ifndef METEOR_OBJFILE_H
#define METEOR_OBJFILE_H

#include <glm/glm.hpp>
#include <vector>
#include <sstream>
#include <map>
#include <cstdint>
#include "Vertex.h"

struct Face {
    std::vector<glm::vec3> vertices {};
    std::vector<glm::vec3> normals {};
    std::vector<glm::vec2> texCoords {};
    uint32_t material_id = -1;
};

class OBJFile {
private:
    struct Point {
        glm::vec3 vertex;
        glm::vec3 normal;
        glm::vec2 uv;
        uint32_t material_id = -1;

        std::string to_string() {
            return std::to_string(vertex.x) + std::to_string(vertex.y) + std::to_string(vertex.z)
                + " " + std::to_string(normal.x) + std::to_string(normal.y) + std::to_string(normal.z)
                + " " + std::to_string(uv.x) + std::to_string(uv.y)
                + " " + std::to_string(material_id);
        }
    };
    using string = std::string;
    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texCoords{};
    std::vector<glm::vec3> tangents{};
    std::vector<uint32_t> material_ids {};
    std::vector<Face> faces{};
    glm::vec3 calculateTangent(Face&, int);

    string material_path = "";
public:
    const string &getMaterialPath() const;

public:
    std::unordered_map<uint32_t, string> material_index_map = std::unordered_map<uint32_t, string>();
    std::unordered_map<string, uint32_t> material_name_map = std::unordered_map<string, uint32_t>();
    static void setStart(uint32_t new_start);
public:
    struct IndexPair {
        std::vector<unsigned int> indices;
        std::vector<Vertex> vertices;
    };

    static uint32_t start;
    static std::optional<uint32_t> oldStart;

    explicit OBJFile(std::istream&);
    static OBJFile fromFilePath(const std::string&);
    std::vector<float> createBuffer();
    std::vector<Vertex> createVulkanBuffer();
    long size() const;

    IndexPair createVulkanBufferIndexed();
};


#endif //METEOR_OBJFILE_H
