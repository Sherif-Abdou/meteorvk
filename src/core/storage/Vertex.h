//
// Created by Sherif Abdou on 8/1/23.
//

#ifndef VULKAN_RENDERER_VERTEX_H
#define VULKAN_RENDERER_VERTEX_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>
#include "VulkanAllocator.h"


struct Vertex {
    constexpr static unsigned int VERTEX_NUM = 5;
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    uint32_t material = -1;

    static vk::VertexInputBindingDescription bindingDescription();
    static std::array<vk::VertexInputAttributeDescription, VERTEX_NUM> attributeDescriptions();
    static void writeVectorToBuffer(const std::vector<Vertex>&, VulkanAllocator::VulkanBufferAllocation&);

    static Vertex positionOnly(glm::vec3);
};


#endif //VULKAN_RENDERER_VERTEX_H
