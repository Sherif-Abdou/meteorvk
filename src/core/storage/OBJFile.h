//
// Created by sheri on 6/20/2023.
//

#ifndef METEOR_OBJFILE_H
#define METEOR_OBJFILE_H

#include <glm/glm.hpp>
#include <vector>
#include <sstream>
#include <map>
#include "Vertex.h"

struct Face {
    std::vector<glm::vec3> vertices {};
    std::vector<glm::vec3> normals {};
    std::vector<glm::vec2> texCoords {};
};

class OBJFile {
private:
    struct Point {
        glm::vec3 vertex;
        glm::vec3 normal;
        glm::vec2 uv;

        std::string to_string() {
            return std::to_string(vertex.x) + std::to_string(vertex.y) + std::to_string(vertex.z)
                + " " + std::to_string(normal.x) + std::to_string(normal.y) + std::to_string(normal.z)
                + " " + std::to_string(uv.x) + std::to_string(uv.y);
        }
    };
    using string = std::string;
    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texCoords{};
    std::vector<glm::vec3> tangents{};
    std::vector<Face> faces{};
    glm::vec3 calculateTangent(Face&, int);
public:
    struct IndexPair {
        std::vector<unsigned int> indices;
        std::vector<Vertex> vertices;
    };
    explicit OBJFile(std::istream&);
    static OBJFile fromFilePath(const std::string&);
    std::vector<float> createBuffer();
    std::vector<Vertex> createVulkanBuffer();
    long size() const;

    IndexPair createVulkanBufferIndexed();
};


#endif //METEOR_OBJFILE_H
