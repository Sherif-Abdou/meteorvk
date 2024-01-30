//
// Created by Sherif Abdou on 1/20/24.
//

#include "GeometryGraphicsPipeline.h"

GeometryGraphicsPipeline::GeometryGraphicsPipeline(std::unique_ptr<ModelBufferGraphicsPipeline> pipeline):
    pipeline(std::move(pipeline)), uniformBuffer(pipeline->getGraphicsPipeline().context){

}

GraphicsPipeline &GeometryGraphicsPipeline::getPipeline() {
    return pipeline->getGraphicsPipeline();
}

void GeometryGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    Renderable::prepareRender(renderArguments);
    uniformBuffer.updateBuffer(ubo);
    uniformBuffer.writeToDescriptor(*descriptorSet);
}

void GeometryGraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    pipeline->renderPipeline(renderArguments);
}

GeometryGraphicsPipeline GeometryGraphicsPipeline::createPipeline(VulkanContext *context) {
    vk::DescriptorSetLayoutBinding uboBinding{};
    uboBinding.setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    vk::DescriptorSetLayoutBinding gBufferBinding {};
    uboBinding.setBinding(1)
        .setDescriptorCount(3)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);


    vk::DescriptorSetLayoutBinding dynamicBufferBinding{};
    dynamicBufferBinding.setBinding(2)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    DescriptorSet* descriptor = new DescriptorSet(context, {uboBinding, gBufferBinding, dynamicBufferBinding});

    //TODO: Make render pass not collapse with multiple output color attachments
    GraphicsRenderPass renderPass(context);
    renderPass.useDepth = true;
    renderPass.storeDepth = false;
    renderPass.useCustomColor(vk::Format::eR16G16B16A16Sfloat, vk::ImageLayout::eColorAttachmentOptimal);
    renderPass.init();

    GraphicsShaders shaders(context, "./shaders/geometry.vert", "./shaders/geometry.frag");
}
