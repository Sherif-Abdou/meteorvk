//
// Created by Sherif Abdou on 9/26/23.
//

#ifndef VULKAN_ENGINE_MODELBUFFER_H
#define VULKAN_ENGINE_MODELBUFFER_H

#include <glm/glm.hpp>
#include "core/VulkanContext.h"
#include "core/storage/DynamicUniformBuffer.h"
#include "engine/material/Material.h"
#include "engine/material/RenderMaterial.h"

/// Buffer that stores model specific uniforms, wrapper over a dynamic uniform
class ModelBuffer {
public:
    /// TODO: Abstract this into a generic
    struct PerModelBuffer {
        glm::mat4 model;
        RenderMaterial material;
    };
private:
    VulkanContext* context;
    unsigned int size;
    unsigned int current_index;
    DynamicUniformBuffer<PerModelBuffer> buffer;
public:
    ModelBuffer(VulkanContext *context, unsigned int size);

    unsigned int getSize() const;

    void updateBuffer(const ModelBuffer::PerModelBuffer &permodel, unsigned int index);

    void writeBuffer(DescriptorSet &descriptorSet, unsigned int binding);

    void resetIndex();
    void nextIndex();

    unsigned long getOffset();

    void attachOffsetToDescriptor(DescriptorSet& descriptorSet, unsigned int index=0);

    void destroy();
};


#endif //VULKAN_ENGINE_MODELBUFFER_H
