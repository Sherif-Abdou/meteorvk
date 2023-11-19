//
// Created by sheri on 6/20/2023.
//

#ifndef METEOR_OBJFILE_H
#define METEOR_OBJFILE_H

#include <glm/glm.hpp>
#include <vector>
#include <sstream>
#include "Vertex.h"

struct Face {
    std::vector<glm::vec3> vertices {};
    std::vector<glm::vec3> normals {};
    std::vector<glm::vec2> texCoords {};
};

class OBJFile {
private:
    using string = std::string;
    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texCoords{};
    std::vector<glm::vec3> tangents{};
    std::vector<Face> faces{};
    glm::vec3 calculateTangent(Face&);
public:
    explicit OBJFile(std::istream&);
    static OBJFile fromFilePath(const std::string&);
    std::vector<float> createBuffer();
    std::vector<Vertex> createVulkanBuffer();
    long size() const;
};


#endif //METEOR_OBJFILE_H
