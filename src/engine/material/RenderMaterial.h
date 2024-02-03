//
// Created by Sherif Abdou on 2/2/24.
//

#ifndef VULKAN_ENGINE_RENDERMATERIAL_H
#define VULKAN_ENGINE_RENDERMATERIAL_H

#include <glm/glm.hpp>

struct RenderMaterial {
    glm::vec3 kA;
    glm::vec3 kD;
    glm::vec3 kS;
    float_t nS;
    uint32_t illum = 1;
    int32_t kD_index = -1;
    int32_t kS_index = -1;
};


#endif //VULKAN_ENGINE_RENDERMATERIAL_H
