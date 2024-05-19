//
// Created by Sherif Abdou on 12/5/23.
//

#include "DepthOnlyPipeline.h"

void DepthOnlyPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    forward_pipeline->renderPipeline(renderArguments);
}

void DepthOnlyPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    Renderable::prepareRender(renderArguments);
    forward_pipeline->prepareRender(renderArguments);
}

DepthOnlyPipeline::DepthOnlyPipeline(std::unique_ptr<ForwardRenderedGraphicsPipeline> forwardPipeline) : forward_pipeline(
        std::move(forwardPipeline))
         {

}

vk::Image DepthOnlyPipeline::getDepthImage() {
    auto pipeline = &forward_pipeline->getPipeline();
    vk::Image image = pipeline->ownedImages[0].imageAllocation.image;
    return image;
}

vk::ImageView DepthOnlyPipeline::getDepthImageView() {
    auto pipeline = &forward_pipeline->getPipeline();
    vk::ImageView image_view = *pipeline->ownedImages[0].imageView;
    return image_view;
}

GraphicsPipeline &DepthOnlyPipeline::getPipeline() {
    return forward_pipeline->getPipeline();
}

ForwardRenderedGraphicsPipeline &DepthOnlyPipeline::getUBO() {
    return *forward_pipeline;
}

void DepthOnlyPipeline::destroy() {
    forward_pipeline->destroy();
}
