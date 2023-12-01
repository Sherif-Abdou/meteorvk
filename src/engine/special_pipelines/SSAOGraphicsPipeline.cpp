//
// Created by Sherif Abdou on 11/27/23.
//

#include "SSAOGraphicsPipeline.h"

#include <random>

#include "../storage/ImageTextureLoader.h"


SSAOGraphicsPipeline::SSAOGraphicsPipeline(ModelBufferGraphicsPipeline& pipeline): pipeline(pipeline), context(pipeline.getGraphicsPipeline().context), ubo_buffer(pipeline.getGraphicsPipeline().context) {
    ubo_buffer.allocateBuffer();
}

void SSAOGraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    pipeline.renderPipeline(renderArguments);
}

void SSAOGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    pipeline.prepareRender(renderArguments);
    ubo_buffer.updateBuffer(ubo);
    ubo_buffer.writeToDescriptor(*descriptor_set);
}

DescriptorSet SSAOGraphicsPipeline::createDescriptorSet(VulkanContext& context, DescriptorSampler* depthSampler) {
    vk::DescriptorSetLayoutBinding buffer_binding{};
    buffer_binding // Uniform Buffer
        .setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    vk::DescriptorSetLayoutBinding depth_binding{};
    depth_binding
        .setBinding(1)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment)
        .setImmutableSamplers(*depthSampler->getSampler());

    vk::DescriptorSetLayoutBinding noise_binding{};
    noise_binding
        .setBinding(2)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    bindings.push_back(buffer_binding);
    bindings.push_back(depth_binding);
    bindings.push_back(noise_binding);
    DescriptorSet set(context, bindings);
    set.buildDescriptor();
    return set;

}

void SSAOGraphicsPipeline::createNoiseImage() {
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }

    ImageTextureLoader imageLoader(context);
    noise_image = imageLoader.createImageFromBuffer(VK_FORMAT_R16G16B16_SFLOAT, 4, 4, ssaoNoise.data(), sizeof(ssaoNoise[0]) * ssaoNoise.size());

    vk::ImageViewCreateInfo image_view_create_info {};
    image_view_create_info.setComponents(vk::ComponentMapping());
    image_view_create_info.setFormat(vk::Format::eR16G16B16Sfloat);
    image_view_create_info.setImage(noise_image.image);
    image_view_create_info.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    image_view_create_info.setViewType(vk::ImageViewType::e2D);

    noise_image_view = context.device.createImageView(image_view_create_info);

    auto sampler = std::make_unique<DescriptorSampler> (context);
    sampler->targetImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    sampler->targetImageView = *noise_image_view;

    noise_sampler.swap(sampler);
}

GraphicsPipeline& SSAOGraphicsPipeline::getPipeline() {
    return pipeline.getGraphicsPipeline();
}
