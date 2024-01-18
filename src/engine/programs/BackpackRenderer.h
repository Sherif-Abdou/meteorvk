//
// Created by Sherif Abdou on 12/10/23.
//

#ifndef VULKAN_ENGINE_BACKPACKRENDERER_H
#define VULKAN_ENGINE_BACKPACKRENDERER_H

#include "../special_pipelines/SSAOGraphicsPipeline.h"
#include "../special_pipelines/ForwardRenderedGraphicsPipeline.h"
#include "../storage/ImageTextureLoader.h"
#include "../special_pipelines/DepthOnlyPipeline.h"
#include "../../core/storage/OBJFile.h"

static constexpr const char *const model_path_1 = "./models/Jeep_Renegade_2016.obj";

class BackpackRenderer {
private:
    std::unique_ptr<ModelBufferGraphicsPipeline> ssao_model_pipeline = nullptr;
    std::unique_ptr<ModelBufferGraphicsPipeline> depth_model_pipeline = nullptr;
    std::unique_ptr<ForwardRenderedGraphicsPipeline> depth_forward_pipeline = nullptr;


    DepthOnlyPipeline createDepthOnlyPipeline(VulkanContext* context, ModelBuffer* modelBuffer, DescriptorSet* descriptorSet);
    std::unique_ptr<SSAOGraphicsPipeline> createSSAOPipeline(VulkanContext* context, DescriptorSet* descriptorSet, ModelBuffer* buffer);

    GraphicsPipeline createShadowPipeline(VulkanContext* context, DescriptorSet* descriptorSet);

    DescriptorSet createUniformBindings(VulkanContext* context);

    CombinedDescriptorSampler createSampler(VulkanContext* context);

    struct TextureResult {
        CombinedDescriptorSampler sampler;
        vk::raii::ImageView view;
    };

    static TextureResult load_texture_from_file(VulkanContext* context, VulkanAllocator::VulkanImageAllocation& red_image);
public:
    void run(VulkanContext* context);
    static VertexBuffer createVertexBuffer(VulkanContext* context, const char* path);
};


#endif //VULKAN_ENGINE_BACKPACKRENDERER_H
