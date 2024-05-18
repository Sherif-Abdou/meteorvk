//
// Created by Sherif Abdou on 5/7/24.
//

#ifndef VULKAN_ENGINE_MINIMALRENDERER_H
#define VULKAN_ENGINE_MINIMALRENDERER_H


#include "../../core/VulkanContext.h"
#include "../storage/ModelBuffer.h"
#include "../../core/storage/VertexBuffer.h"
#include "../../core_v2/GraphicsPipelineBuilder2.h"
#include "../../core/graphics_pipeline/GraphicsCommandBuffer.h"
#include "../../core/storage/NewOBJFile.h"

class MinimalRenderer {
private:
    VulkanContext* context;
    ModelBuffer* buffer;

    const std::string ubo_name = "ubo";

    NewDescriptorManager* descriptorManager;
public:
    explicit MinimalRenderer(VulkanContext* context) : context(context) {}

    void run();

    VertexBuffer createVertexBuffer(VulkanContext *context, const char *path);

    GraphicsPipeline buildPipeline();
};


#endif //VULKAN_ENGINE_MINIMALRENDERER_H
