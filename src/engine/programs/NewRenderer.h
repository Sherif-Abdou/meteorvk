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
#include "../special_pipelines/DepthOnlyPipeline.h"
#include "../special_pipelines/SSAOGraphicsPipeline.h"
#include "../special_pipelines/ShadowGraphicsPipeline.h"
#include "../../core_v2/render_chain/RenderableChain.h"
#include "../storage/LightBuffer.h"

class NewRenderer {
private:
    VulkanContext* context;
    ModelBuffer* model_buffer = nullptr;

    float sensitivity = 1.0f;

    std::unique_ptr<ForwardRenderedGraphicsPipeline> pipeline = nullptr;

    std::unique_ptr<NewDescriptorManager> descriptorManager = nullptr;

    std::unique_ptr<CombinedDescriptorSampler> depth_sampler = nullptr;
    std::unique_ptr<CombinedDescriptorSampler> occlusion_sampler = nullptr;
    std::unique_ptr<CombinedDescriptorSampler> shadow_sampler = nullptr;

    std::unique_ptr<SSAOGraphicsPipeline> ssao_pipeline = nullptr;
    std::unique_ptr<DepthOnlyPipeline> depth_pipeline = nullptr;
    std::unique_ptr<ShadowGraphicsPipeline> shadow_pipeline = nullptr;


    std::unique_ptr<LightBuffer> light_buffer = nullptr;
public:
    explicit NewRenderer(VulkanContext* context): context(context) {};

    void run();

    void tick(double elapsed);

    void buildDescriptorLayouts();
    std::unique_ptr<ForwardRenderedGraphicsPipeline> buildForwardGraphicsPipeline();
    std::unique_ptr<DepthOnlyPipeline> buildDepthOnlyPipeline();
    std::unique_ptr<SSAOGraphicsPipeline> buildSSAOGraphicsPipeline(vk::ImageView depth_image_view);
    std::unique_ptr<ShadowGraphicsPipeline> buildShadowGraphicsPipeline();

    VertexBuffer createVertexBuffer(VulkanContext *context, const char *path);

    void buildLighting();
};

#endif
