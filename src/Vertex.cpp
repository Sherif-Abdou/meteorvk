//
// Created by Sherif Abdou on 8/1/23.
//

#include "Vertex.h"

vk::VertexInputBindingDescription Vertex::bindingDescription() {
    auto binding =  VkVertexInputBindingDescription();
    binding.binding = 0;
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    binding.stride = sizeof (Vertex);
    return binding;
}

std::array<vk::VertexInputAttributeDescription, Vertex::VERTEX_NUM> Vertex::attributeDescriptions() {
    auto position_attribute = VkVertexInputAttributeDescription();
    position_attribute.binding = 0;
    position_attribute.location = 0;
    position_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    position_attribute.offset = offsetof(Vertex, position);

    auto normal_attribute = VkVertexInputAttributeDescription();
    normal_attribute.binding = 0;
    normal_attribute.location = 1;
    normal_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    normal_attribute.offset = offsetof(Vertex, normal);

    auto tex_attribute = VkVertexInputAttributeDescription();
    tex_attribute.binding = 0;
    tex_attribute.location = 2;
    tex_attribute.format = VK_FORMAT_R32G32_SFLOAT;
    tex_attribute.offset = offsetof(Vertex, texCoord);

    auto tangent_attribute = VkVertexInputAttributeDescription();
    tangent_attribute.binding = 0;
    tangent_attribute.location = 3;
    tangent_attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    tangent_attribute.offset = offsetof(Vertex, tangent);

    std::array<vk::VertexInputAttributeDescription, 4> all = {position_attribute, normal_attribute, tex_attribute, tangent_attribute};
    std::array<vk::VertexInputAttributeDescription, VERTEX_NUM> final {};
    for (int i = 0; i < VERTEX_NUM; i++) {
        final[i] = all[i];
    }

    return final;
}

void Vertex::writeVectorToBuffer(const std::vector<Vertex> & values, VulkanAllocator::VulkanBufferAllocation & buffer) {
    void* memory = buffer.mapMemory();
        auto bytes = sizeof(values[0]) * values.size();
        memcpy(memory, values.data(), bytes);
    buffer.unmapMemory();
}

Vertex Vertex::positionOnly(glm::vec3 position) {
    auto vertex = Vertex();
    vertex.position = position;

    return vertex;
}
