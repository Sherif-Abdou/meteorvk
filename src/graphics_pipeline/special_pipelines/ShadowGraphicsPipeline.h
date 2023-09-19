//
// Created by Sherif Abdou on 9/15/23.
//

#ifndef VULKAN_ENGINE_SHADOWGRAPHICSPIPELINE_H
#define VULKAN_ENGINE_SHADOWGRAPHICSPIPELINE_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include "../GraphicsPipeline.h"
#include "../../storage/UniformBuffer.h"
#include <glm/ext.hpp>

class ShadowGraphicsPipeline: public Renderable {
private:
    GraphicsPipeline pipeline;
public:
    GraphicsPipeline &getPipeline();
    DescriptorSet* descriptorSet = nullptr;
    uint32_t binding = 0;
    struct UBO {
        glm::mat4 proj;
        glm::mat4 view;
        glm::mat4 model;
    };

    UBO lightUBO = {
            glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 20.0f),
            glm::lookAt(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0, 0.0, 1.0)),
            glm::identity<glm::mat4>(),
    };

    UniformBuffer<UBO> lightUniformBuffer;

    ShadowGraphicsPipeline(VulkanContext &context, GraphicsRenderPass &&renderPass);

    ShadowGraphicsPipeline(GraphicsPipeline &&pipeline);

    void renderPipeline(Renderable::RenderArguments renderArguments) override;
};


#endif //VULKAN_ENGINE_SHADOWGRAPHICSPIPELINE_H
