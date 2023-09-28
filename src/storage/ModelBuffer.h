//
// Created by Sherif Abdou on 9/26/23.
//

#ifndef VULKAN_ENGINE_MODELBUFFER_H
#define VULKAN_ENGINE_MODELBUFFER_H

#include <glm/glm.hpp>
#include "../VulkanContext.h"
#include "DynamicUniformBuffer.h"

class ModelBuffer {
public:
    struct PerModelBuffer {
        glm::mat4 model;
    };
private:
    VulkanContext& context;
    unsigned int size;
    DynamicUniformBuffer<PerModelBuffer> buffer;

public:
    ModelBuffer(VulkanContext &context, unsigned int size);

    unsigned int getSize() const;

    void updateBuffer(PerModelBuffer& permodel, unsigned int index);
};


#endif //VULKAN_ENGINE_MODELBUFFER_H
