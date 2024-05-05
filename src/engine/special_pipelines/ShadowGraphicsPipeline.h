//
// Created by Sherif Abdou on 9/15/23.
//

#ifndef VULKAN_ENGINE_SHADOWGRAPHICSPIPELINE_H
#define VULKAN_ENGINE_SHADOWGRAPHICSPIPELINE_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include "../../core/graphics_pipeline/GraphicsPipeline.h"
#include "../../core/storage/UniformBuffer.h"
#include "ModelBufferGraphicsPipeline.h"
#include <glm/ext.hpp>

/// Graphics pipeline meant for generating a shadow map
class ShadowGraphicsPipeline: public BasePipeline {
private:
    std::unique_ptr<ModelBufferGraphicsPipeline> pipeline;
public:
    GraphicsPipeline &getPipeline();
    GraphicsPipeline &getGraphicsPipeline() override;

    void setDescriptorSet(DescriptorSet* descriptor);


    uint32_t binding = 0;
    struct UBO {
        glm::mat4 proj;
        glm::mat4 view;
        glm::mat4 model;
    };

    UBO lightUBO = {
            glm::ortho(-20.f, 20.f, -20.f, 20.f, 0.1f, 20.0f),
            glm::lookAt(glm::vec3(-5.0f, 5.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.0, 0.0, 1.0)),
            glm::identity<glm::mat4>(),
    };

    UniformBuffer<UBO> lightUniformBuffer;


    explicit ShadowGraphicsPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline);


    void renderPipeline(Renderable::RenderArguments renderArguments) override;

    void prepareRender(Renderable::RenderArguments renderArguments) override;

    virtual ~ShadowGraphicsPipeline();

    void destroy() override;
};


#endif //VULKAN_ENGINE_SHADOWGRAPHICSPIPELINE_H
