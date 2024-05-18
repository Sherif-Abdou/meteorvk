#ifndef VULKAN_ENGINE_NEW_RENDERER_H
#define VULKAN_ENGINE_NEW_RENDERER_H

#include "../../core/VulkanContext.h"
#include "../storage/ModelBuffer.h"
#include "../../core/storage/VertexBuffer.h"
#include "../../core_v2/GraphicsPipelineBuilder2.h"
#include "../../core/graphics_pipeline/GraphicsCommandBuffer.h"
#include "../../core/storage/NewOBJFile.h"
#include "../special_pipelines/ModelBufferGraphicsPipeline.h"
#include "../special_pipelines/ForwardRenderedGraphicsPipeline.h"
#include "../../core_v2/render_chain/RenderableChain.h"

class NewRenderer {
private:
    VulkanContext* context;
    ModelBuffer* model_buffer = nullptr;

    float sensitivity = 1.0f;

    std::unique_ptr<ForwardRenderedGraphicsPipeline> pipeline = nullptr;

    std::unique_ptr<NewDescriptorManager> descriptorManager = nullptr;
public:
    explicit NewRenderer(VulkanContext* context): context(context) {};

    void run();

    void tick(double elapsed);

    void buildDescriptorLayouts();
    std::unique_ptr<ForwardRenderedGraphicsPipeline> buildForwardGraphicsPipeline();

    VertexBuffer createVertexBuffer(VulkanContext *context, const char *path);
};

#endif
