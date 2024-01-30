//
// Created by Sherif Abdou on 1/20/24.
//

#ifndef VULKAN_ENGINE_GEOMETRYGRAPHICSPIPELINE_H
#define VULKAN_ENGINE_GEOMETRYGRAPHICSPIPELINE_H


#include "../../core/interfaces/Renderable.h"
#include "ModelBufferGraphicsPipeline.h"
#include "glm/ext.hpp"

class GeometryGraphicsPipeline: public Renderable {
    explicit GeometryGraphicsPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline);

    void renderPipeline(Renderable::RenderArguments renderArguments) override;

    struct UBO {
        glm::mat4 proj;
        glm::mat4 view;
    };

    UBO ubo {
            glm::identity<glm::mat4>(),
            glm::identity<glm::mat4>(),
    };

    UniformBuffer<UBO> uniformBuffer;

    DescriptorSet* descriptorSet = nullptr;
    GraphicsPipeline &getPipeline();

    void prepareRender(Renderable::RenderArguments renderArguments) override;

    static GeometryGraphicsPipeline createPipeline(VulkanContext* context);

    void destroy();
private:
    std::unique_ptr<ModelBufferGraphicsPipeline> pipeline;
};


#endif //VULKAN_ENGINE_GEOMETRYGRAPHICSPIPELINE_H
