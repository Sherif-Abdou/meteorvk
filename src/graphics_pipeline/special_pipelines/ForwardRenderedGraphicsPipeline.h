//
// Created by Sherif Abdou on 9/18/23.
//

#ifndef VULKAN_ENGINE_FORWARDRENDEREDGRAPHICSPIPELINE_H
#define VULKAN_ENGINE_FORWARDRENDEREDGRAPHICSPIPELINE_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "../GraphicsPipeline.h"
#include "../../interfaces/Renderable.h"
#include "../../storage/UniformBuffer.h"

class ForwardRenderedGraphicsPipeline: public Renderable {
public:
    explicit ForwardRenderedGraphicsPipeline(GraphicsPipeline &&pipeline);

    void renderPipeline(Renderable::RenderArguments renderArguments) override;
    struct UBO {
        glm::mat4 proj;
        glm::mat4 view;
        glm::mat4 model;
    };

    UBO ubo {
        glm::identity<glm::mat4>(),
        glm::identity<glm::mat4>(),
        glm::identity<glm::mat4>(),
    };

    UniformBuffer<UBO> uniformBuffer;

    DescriptorSet* descriptorSet = nullptr;
    GraphicsPipeline &getPipeline();
private:
    GraphicsPipeline pipeline;
};


#endif //VULKAN_ENGINE_FORWARDRENDEREDGRAPHICSPIPELINE_H