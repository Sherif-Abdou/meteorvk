//
// Created by Sherif Abdou on 12/5/23.
//

#ifndef VULKAN_ENGINE_DEPTHONLYPIPELINE_H
#define VULKAN_ENGINE_DEPTHONLYPIPELINE_H


#include "../../core/interfaces/Renderable.h"
#include "ForwardRenderedGraphicsPipeline.h"

class DepthOnlyPipeline: public Renderable {
private:
    ForwardRenderedGraphicsPipeline forward_pipeline;
    VulkanContext& context;
public:
    explicit DepthOnlyPipeline(ForwardRenderedGraphicsPipeline &forwardPipeline);

    void renderPipeline(Renderable::RenderArguments renderArguments) override;

    void prepareRender(Renderable::RenderArguments renderArguments) override;

    static DepthOnlyPipeline createDepthOnlyPipeline(GraphicsPipelineBuilder&& builder, ModelBuffer* models, DescriptorSet* descriptor_set);

    vk::Image getDepthImage();
    vk::ImageView getDepthImageView();

    ForwardRenderedGraphicsPipeline& getUBO();

    GraphicsPipeline &getPipeline();

    void destroy();
};


#endif //VULKAN_ENGINE_DEPTHONLYPIPELINE_H
