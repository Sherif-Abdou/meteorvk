#ifndef VULKAN_ENGINE_LIGHT_BUFFER_H
#define VULKAN_ENGINE_LIGHT_BUFFER_H

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include "../../core/storage/UniformBuffer.h"
#include "../../core_v2/NewDescriptorManager.h"
#include <vulkan/vulkan.hpp>
#include <glm/ext.hpp>

class LightBuffer {
public:
    static constexpr size_t MAX_LIGHTS = 64;
    static constexpr const char* LIGHT_UBO_NAME = "light_ubo";
private:
    VulkanContext* context;

    struct UBO {
        glm::vec4 light_positions[64];
        glm::mat4 light_povs[64];
        int32_t light_count;
    };

    std::unique_ptr<UBO> ubo;
    std::unique_ptr<UniformBuffer<UBO>> ubo_buffer;

public:
    explicit LightBuffer(VulkanContext* context);

    void updateDescriptor(DescriptorSet* descriptor, uint32_t binding);
    void updateDescriptor(NewDescriptorManager* descriptorManager);

    void addLayoutBinding(NewDescriptorManager* descriptorManager);

    uint32_t addLight(glm::vec3 position, glm::mat4 pov);

    ~LightBuffer();
};

#endif
