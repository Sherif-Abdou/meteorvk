//
// Created by Sherif Abdou on 11/27/23.
//

#ifndef SSAOGRAPHICSPIPELINE_H
#define SSAOGRAPHICSPIPELINE_H
#include "ModelBufferGraphicsPipeline.h"
#include "../../core/graphics_pipeline/GraphicsPipeline.h"
#include "../../core/interfaces/Renderable.h"
#include "../../core/storage/CombinedDescriptorSampler.h"


class SSAOGraphicsPipeline: public Renderable {

public:
    explicit SSAOGraphicsPipeline(ModelBufferGraphicsPipeline &pipeline);

    void renderPipeline(Renderable::RenderArguments renderArguments) override;
    void prepareRender(Renderable::RenderArguments renderArguments) override;

    static DescriptorSet createDescriptorSet(VulkanContext& context, CombinedDescriptorSampler* depthSampler);

    void createNoiseImage();

    struct UBO {
        glm::mat4 proj;
        glm::mat4 view;
        glm::vec3 samples[64];
    };

    UBO ubo;
    UniformBuffer<SSAOGraphicsPipeline::UBO> ubo_buffer;
    DescriptorSet* descriptor_set;
    GraphicsPipeline& getPipeline();
    VulkanContext& context;
private:
    ModelBufferGraphicsPipeline& pipeline;
    VulkanAllocator::VulkanImageAllocation noise_image;
    vk::raii::ImageView noise_image_view = nullptr;
    std::unique_ptr<CombinedDescriptorSampler> noise_sampler = nullptr;
};



#endif //SSAOGRAPHICSPIPELINE_H
