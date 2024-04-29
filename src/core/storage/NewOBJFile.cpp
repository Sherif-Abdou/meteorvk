//
// Created by Sherif Abdou on 4/29/24.
//

#include "NewOBJFile.h"

namespace std {
    template<>
    struct hash<NewOBJFile::Face> {
        std::size_t operator()(const NewOBJFile::Face& f) const {
            std::size_t res = 0;
            for (const auto& item: f.vertexIndices) {
                boost::hash_combine(res, item);
            }
            boost::hash_combine(res, -1);
            for (const auto& item: f.normalIndices) {
                boost::hash_combine(res, item);
            }
            boost::hash_combine(res, -1);
            for (const auto& item: f.vertexIndices) {
                boost::hash_combine(res, item);
            }
            boost::hash_combine(res, -1);
            boost::hash_combine(res, f.material_id);

            return res;
        }
    };

    template<> struct hash<NewOBJFile::Point> {
        std::size_t operator()(const NewOBJFile::Point& f) const {
            std::size_t res = 0;
            boost::hash_combine(res, f.vertexIndex);
            boost::hash_combine(res, f.normalIndex);
            boost::hash_combine(res, f.texCoordIndex);
            boost::hash_combine(res, f.material_id);

            return res;
        }
    };
}

NewOBJFile::NewOBJFile(std::ifstream &in) {
    std::string str;
    uint32_t max_index = -1;
    uint32_t current_index = -1;

    while (std::getline(in, str))  {
        std::stringstream stringstream {str};
        std::string opener;
        stringstream >> opener;
        if (opener == "mtllib") {
            std::string material_path;
            stringstream >> material_path;

            mtl_paths.push_back(material_path);
        } else if (opener == "vt") {
            float u;
            float v;
            stringstream >> u;
            stringstream >> v;
            texCoords.emplace_back(u, v);
        } else if (opener == "vn") {
            float x, y, z;
            stringstream >> x >> y >> z;
            normals.emplace_back(x, y, z);
        } else if (opener == "v") {
            float x, y, z;
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
                face.vertexIndices.push_back(vertex_index - 1);
                face.normalIndices.push_back(normal_index - 1);
                face.texCoordIndices.push_back(tex_index - 1);
                face.material_id = current_index;
            } while (!stringstream.eof());
            faces.push_back(face);
        } else if (opener == "usemtl") {
            std::string mtl_name;
            stringstream >> mtl_name;

            if (mtl_name_map.find(mtl_name) != mtl_name_map.end()) {
                current_index = mtl_name_map[mtl_name];
            } else {
                max_index += 1;
                mtl_name_map[mtl_name] = max_index;
                mtl_index_map[max_index] = mtl_name;
                current_index = max_index;
            }
        }
    }
}

glm::vec3 NewOBJFile::calculateTangent(const NewOBJFile::Face &face, int offset) {
    auto pos = vertices[face.vertexIndices[0 + offset]];
    auto pos2 = vertices[face.vertexIndices[1 + offset]];
    auto pos3 = vertices[face.vertexIndices[2 + offset]];
    auto uv = texCoords[face.texCoordIndices[0 + offset]];
    auto uv2 = texCoords[face.texCoordIndices[1 + offset]];
    auto uv3 = texCoords[face.texCoordIndices[2 + offset]];

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

NewOBJFile::IndexPair NewOBJFile::exportIndexedBuffer() {
    auto vertices = std::vector<Vertex>();
    auto indices = std::vector<unsigned int>();
    auto existing_map = std::unordered_map<size_t, unsigned int>();

    for (auto& face: faces) {
        // Triangle fan behavior goes [0, 1, 2], [0, 2, 3], [0, 3, 4] ...
        for (int offset = 1; offset <= face.vertexIndices.size() -2; offset++) {
            auto tangent = calculateTangent(face, 0);
            auto vertex = face.vertexIndices[0];
            auto normal = face.normalIndices[0];
            auto tex = face.texCoordIndices[0];

            Point p = {vertex, normal, tex, face.material_id};

            // Don't allocate another vertex if we already have one stored
            if (existing_map.find(std::hash<Point>()(p)) == existing_map.end()) {
                existing_map[std::hash<Point>()(p)] = existing_map.size();
                vertices.push_back(Vertex {
                        this->vertices[vertex],
                        normals[normal],
                        texCoords[tex],
                        tangent,
                        face.material_id,
                });
            }

            auto index_of_point = existing_map[std::hash<Point>()(p)];
            indices.push_back(index_of_point);
            for (int i = offset; i < offset + 2; i++) {
                auto vertex = face.vertexIndices[i];
                auto normal = face.normalIndices[i];
                auto tex = face.texCoordIndices[i];

                Point p = {vertex, normal, tex, face.material_id};
                if (existing_map.find(std::hash<Point>()(p)) == existing_map.end()) {
                    existing_map[std::hash<Point>()(p)] = existing_map.size();
                    vertices.push_back(Vertex {
                            this->vertices[vertex],
                            normals[normal],
                            texCoords[tex],
                            tangent,
                            face.material_id,
                    });
                }
                auto index_of_point = existing_map[std::hash<Point>()(p)];
                indices.push_back(index_of_point);
            }
        }
    }

    return {indices, vertices};
}

NewOBJFile NewOBJFile::fromFilePath(const std::string &file_path) {
    std::ifstream file (file_path);
    return NewOBJFile(file);
}

const std::vector<std::string> &NewOBJFile::getMaterialPaths() const {
    return this->mtl_paths;
}

const std::unordered_map<uint32_t, std::string> &NewOBJFile::getMTLIndexMap() const {
  return this->mtl_index_map;
}
