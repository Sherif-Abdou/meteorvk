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
    SSAOGraphicsPipeline(VulkanContext* context, ModelBufferGraphicsPipeline *input_pipeline, DescriptorSet* set = nullptr);
//    SSAOGraphicsPipeline::SSAOGraphicsPipeline(VulkanContext& context, ModelBufferGraphicsPipeline* input_pipeline) {
//    }

    void renderPipeline(Renderable::RenderArguments renderArguments) override;
    void prepareRender(Renderable::RenderArguments renderArguments) override;

    static DescriptorSet createDescriptorSet(VulkanContext& context, CombinedDescriptorSampler* depthSampler);


    struct UBO {
        glm::mat4 proj;
        glm::mat4 view;
        glm::vec3 samples[64];
    };

    std::unique_ptr<UBO> ubo = nullptr;
    std::unique_ptr<UniformBuffer<SSAOGraphicsPipeline::UBO>> ubo_buffer = nullptr;
    DescriptorSet* descriptor_set{};
    GraphicsPipeline& getPipeline();
    CombinedDescriptorSampler* depth_sampler{};

    CombinedDescriptorSampler *getDepthSampler() const;

    void setDepthSampler(CombinedDescriptorSampler *depthSampler);


    VulkanContext* context;

    void destroy();
    vk::ImageView getOcclusionImageView();
    vk::Image getOcclusionImage();

    void init();

    ~SSAOGraphicsPipeline() override;

private:
    void createNoiseImage();
    void createSamples();
    ModelBufferGraphicsPipeline& pipeline;
    VulkanAllocator::VulkanImageAllocation noise_image;
    vk::raii::ImageView noise_image_view = nullptr;
    std::unique_ptr<CombinedDescriptorSampler> noise_sampler;

    float lerp(float a, float b, float f);

};



#endif //SSAOGRAPHICSPIPELINE_H
