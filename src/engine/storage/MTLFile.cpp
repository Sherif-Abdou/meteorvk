//
// Created by Sherif Abdou on 2/2/24.
//

#include "MTLFile.h"

MTLFile::MTLFile(std::istream &stream) {
    string str {};

    while (std::getline(stream, str)) {
        std::stringstream stringstream {str};
        string opener;
        stringstream >> opener;
        if (opener == "newmtl") {
            string name;
            stringstream >> name;
            if (current_material.has_value()) {
                render_materials[*current_material_name] = *current_material;
            }
            current_material = RenderMaterial {};
            current_material_name = name;
        }
        if (opener == "Ka") {
            float r, g, b;
            stringstream >> r >> g >> b;

            current_material->kA = {r, g, b};
        }
        if (opener == "Kd") {
            float r, g, b;
            stringstream >> r >> g >> b;

            current_material->kD = {r, g, b};
        }
        if (opener == "Ks") {
            float r, g, b;
            stringstream >> r >> g >> b;

            current_material->kS = {r, g, b};
        }
        if (opener == "Ns") {
            float s;
            stringstream >> s;

            current_material->nS = s;
        }
        if (opener == "illum") {
            uint32_t s;
            stringstream >> s;

            current_material->illum = s;
        }
    }
    if (current_material.has_value()) {
        render_materials[*current_material_name] = *current_material;
    }
}

const std::unordered_map<std::string, RenderMaterial> &MTLFile::getRenderMaterials() const {
    return render_materials;
}

MTLFile MTLFile::fromFilePath(const std::string & path) {
    std::ifstream stream(path);
    return MTLFile(stream);
}
