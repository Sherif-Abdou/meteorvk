//
// Created by Sherif Abdou on 1/19/24.
//

#ifndef VULKAN_ENGINE_POSTGRAPHICSPIPELINE_H
#define VULKAN_ENGINE_POSTGRAPHICSPIPELINE_H


#include "core/VulkanContext.h"
#include "ModelBufferGraphicsPipeline.h"
#include "glm/ext.hpp"

class PostGraphicsPipeline: public Renderable {
public:
    struct UBO {
        glm::mat4 proj;
        glm::mat4 view;
    };
    UBO ubo;
private:
    VulkanContext* context;
    std::unique_ptr<ModelBufferGraphicsPipeline> pipeline;
    UniformBuffer<UBO> uniformBuffer;
public:
    explicit PostGraphicsPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline);

    DescriptorSet* descriptorSet = nullptr;
    GraphicsPipeline &getPipeline();

    void renderPipeline(Renderable::RenderArguments renderArguments) override;

    void prepareRender(Renderable::RenderArguments renderArguments) override;
};


#endif //VULKAN_ENGINE_POSTGRAPHICSPIPELINE_H
