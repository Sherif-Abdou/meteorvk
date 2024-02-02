//
// Created by sheri on 6/20/2023.
//

#include "OBJFile.h"
#include <string>
#include <sstream>
#include <fstream>


OBJFile::OBJFile(std::istream& stream) {
    string str {};

    while (std::getline(stream, str)) {
        // Texture line
        std::stringstream stringstream {str};
        string opener;
        stringstream >> opener;
        if (opener == "vt") {
            float u;
            float v;
            stringstream >> u;
            stringstream >> v;
            texCoords.emplace_back(u, v);
        } else if (opener == "vn") {
            float x;
            float y;
            float z;
            stringstream >> x >> y >> z;
            normals.emplace_back(x, y, z);
        } else if (opener == "v") {
            float x;
            float y;
            float z;
            stringstream >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        } else if (opener == "f") {
            Face face {};
            unsigned int vertex_index;
            unsigned int tex_index;
            unsigned int normal_index;
            char buf;
            do {
                stringstream >> vertex_index >> buf >> tex_index >> buf >> normal_index;
                face.vertices.push_back(vertices[vertex_index-1]);
                face.normals.push_back(normals[normal_index-1]);
                if (tex_index != 0) {
                    face.texCoords.push_back(texCoords[tex_index-1]);
                } else {
                    while (buf != ' ') {
                        stringstream >> buf;
                    }
//                    break;
                }
            } while (!stringstream.eof());
            faces.push_back(face);
        } else if (opener == "o") {

        }
    }
}

std::vector<float> OBJFile::createBuffer() {
    auto buffer = std::vector<float>();
    for (auto& face : faces) {
        for (int offset = 0; offset < face.vertices.size()-2; offset++) {
            auto tangent = calculateTangent(face, offset);
            for (int i = offset; i < face.vertices.size(); i++) {
                auto &vertex = face.vertices[i];
                auto &normal = face.normals[i];
                auto &tex = face.texCoords[i];
                buffer.push_back(vertex.x);
                buffer.push_back(vertex.y);
                buffer.push_back(vertex.z);
                buffer.push_back(normal.x);
                buffer.push_back(normal.y);
                buffer.push_back(normal.z);
                buffer.push_back(tangent.x);
                buffer.push_back(tangent.y);
                buffer.push_back(tangent.z);
                buffer.push_back(tex.x);
                buffer.push_back(tex.y);
            }
        }
    }
    return buffer;
}

long OBJFile::size() const {
    int i = 0;

    for (auto& face: faces) {
        i += face.vertices.size();
    }

    return i;
}

glm::vec3 OBJFile::calculateTangent(Face & face, int offset) {
    auto pos = face.vertices[0 + offset];
    auto pos2 = face.vertices[1 + offset];
    auto pos3 = face.vertices[2 + offset];
    auto uv = face.texCoords[0 + offset];
    auto uv2 = face.texCoords[1 + offset];
    auto uv3 = face.texCoords[2 + offset];

    glm::vec3 tangent {};

    auto edge1 = pos2 - pos;
    auto edge2 = pos3 - pos;
    auto deltaUV1 = uv2 - uv;
    auto deltaUV2 = uv3 - uv;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    return tangent;
}

std::vector<Vertex> OBJFile::createVulkanBuffer() {
    auto buffer = std::vector<Vertex>();
    for (auto& face : faces) {
        for (int offset = 0; offset < face.vertices.size() - 2; offset++) {
            auto tangent = calculateTangent(face, offset);
            for (int i = offset; i < offset + 3; i++) {
                auto& vertex = face.vertices[i];
                auto& normal = face.normals[i];
                auto& tex = face.texCoords[i];
                buffer.push_back(Vertex {
                        vertex,
                        normal,
                        tex,
                        tangent
                });
            }
        }
    }
    return buffer;
}

OBJFile::IndexPair OBJFile::createVulkanBufferIndexed() {
    auto v = std::vector<Vertex>();
    auto indices = std::vector<unsigned int>();
    auto point_map = std::map<std::string, unsigned int>();
    for (auto& face : faces) {

        for (int offset = 1; offset <= face.vertices.size() - 2; offset++) {
            auto tangent = calculateTangent(face, 0);
            auto& vertex = face.vertices[0];
            auto& normal = face.normals[0];
            glm::vec2 tex;
            tex = face.texCoords[0];
//                if (i >= face.texCoords.size()) {
//                    tex = {0, 0};
//                } else {
//                }
            Point p = {vertex, normal, tex};

            if (point_map.find(p.to_string()) == point_map.end()) {
                point_map[p.to_string()] = v.size();
                v.push_back(Vertex {
                        vertex,
                        normal,
                        tex,
                        tangent
                });
            }

            auto res = point_map[p.to_string()];
            indices.push_back(res);
            if (point_map.size() != v.size()) {
                throw std::exception();
            }
            for (int i = offset; i < offset + 2; i++) {
                auto& vertex = face.vertices[i];
                auto& normal = face.normals[i];
                glm::vec2 tex;
                tex = face.texCoords[i];
//                if (i >= face.texCoords.size()) {
//                    tex = {0, 0};
//                } else {
//                }
                Point p = {vertex, normal, tex};

                if (point_map.find(p.to_string()) == point_map.end()) {
                    point_map[p.to_string()] = v.size();
                    v.push_back(Vertex {
                            vertex,
                            normal,
                            tex,
                            tangent
                    });
                }

                auto res = point_map[p.to_string()];
                indices.push_back(res);
                if (point_map.size() != v.size()) {
                    throw std::exception();
                }
            }
        }
    }
    return {indices, v};
}

OBJFile OBJFile::fromFilePath(const std::string & file_path) {
    std::ifstream file (file_path);
    return OBJFile(file);
}


