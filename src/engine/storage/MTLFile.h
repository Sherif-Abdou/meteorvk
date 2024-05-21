//
// Created by Sherif Abdou on 2/2/24.
//

#ifndef VULKAN_ENGINE_MTLFILE_H
#define VULKAN_ENGINE_MTLFILE_H

#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <optional>
#include <unordered_map>
#include "engine/material/RenderMaterial.h"

class MTLFile {
    using string = std::string;
    std::unordered_map<string, RenderMaterial> render_materials {};
public:
    const std::unordered_map<string, RenderMaterial> &getRenderMaterials() const;

private:
    std::optional<RenderMaterial> current_material;
    std::optional<string> current_material_name;
    std::string path;
    static int image_index;

public:
    explicit MTLFile(std::istream& stream);
    static MTLFile fromFilePath(const std::string&);
};


#endif //VULKAN_ENGINE_MTLFILE_H
