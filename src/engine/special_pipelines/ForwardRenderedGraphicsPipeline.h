//
// Created by Sherif Abdou on 9/18/23.
//

#ifndef VULKAN_ENGINE_FORWARDRENDEREDGRAPHICSPIPELINE_H
#define VULKAN_ENGINE_FORWARDRENDEREDGRAPHICSPIPELINE_H

#include "BasePipeline.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "core/graphics_pipeline/GraphicsPipeline.h"
#include "core/interfaces/Renderable.h"
#include "core/storage/UniformBuffer.h"
#include "ModelBufferGraphicsPipeline.h"

/// Graphics pipelines that stores render specific uniforms
class ForwardRenderedGraphicsPipeline: public BasePipeline {
public:
    static constexpr const char* FORWARD_UBO_NAME = "global_ubo";

    explicit ForwardRenderedGraphicsPipeline(std::unique_ptr<BasePipeline> pipeline);

    void renderPipeline(Renderable::RenderArguments renderArguments) override;
    struct UBO {
        glm::mat4 proj;
        glm::mat4 view;
        glm::mat4 lightProjView;
    };

    UBO ubo {
        glm::identity<glm::mat4>(),
        glm::identity<glm::mat4>(),
        glm::zero<glm::mat4>(),
    };

    UniformBuffer<UBO> uniformBuffer;

    void setDescriptorSet(DescriptorSet* descriptor);

    GraphicsPipeline &getPipeline();
    GraphicsPipeline &getGraphicsPipeline() override;

    static ForwardRenderedGraphicsPipeline* createFromModelPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline);

    void prepareRender(Renderable::RenderArguments renderArguments) override;

    void destroy() override;
private:
    std::unique_ptr<BasePipeline> pipeline;
};


#endif //VULKAN_ENGINE_FORWARDRENDEREDGRAPHICSPIPELINE_H
