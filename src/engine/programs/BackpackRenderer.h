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
#include "../../core/storage/DescriptorSampler.h"
#include "../../core/storage/StorageImage.h"
#include "../storage/TextureContainer.h"
#include "../storage/TextureDescriptorSet.h"
#include <filesystem>
#include <paths.h>

static constexpr const char *const model_path_1 = "./models/non_triangled_senna.obj";

class BackpackRenderer {
private:
    std::unique_ptr<ModelBufferGraphicsPipeline> ssao_model_pipeline = nullptr;
    std::unique_ptr<ModelBufferGraphicsPipeline> depth_model_pipeline = nullptr;
    std::unique_ptr<ForwardRenderedGraphicsPipeline> depth_forward_pipeline = nullptr;
    DescriptorSampler* textureSampler = nullptr;

//    StorageImage* images[2];
    TextureContainer textureContainer;
//    TextureContainer containers;

    DepthOnlyPipeline createDepthOnlyPipeline(VulkanContext* context, ModelBuffer* modelBuffer, DescriptorSet* descriptorSet);
    std::unique_ptr<SSAOGraphicsPipeline> createSSAOPipeline(VulkanContext* context, DescriptorSet* descriptorSet, ModelBuffer* buffer);

    GraphicsPipeline createShadowPipeline(VulkanContext* context, DescriptorSet* descriptorSet);

    DescriptorSet createUniformBindings(VulkanContext* context);

    CombinedDescriptorSampler createSampler(VulkanContext* context);

    TextureDescriptorSet* createTextureDescriptor(VulkanContext* context);

    struct TextureResult {
        CombinedDescriptorSampler sampler;
        vk::raii::ImageView view;
    };

    static TextureResult load_texture_from_file(VulkanContext* context, VulkanAllocator::VulkanImageAllocation& red_image);
public:
    void run(VulkanContext* context);
    VertexBuffer createVertexBuffer(VulkanContext* context, const char* path);

    void addTexture(VulkanContext *context, TextureDescriptorSet *textureSet, const char* path);
};


#endif //VULKAN_ENGINE_BACKPACKRENDERER_H
