//
// Created by Sherif Abdou on 11/27/23.
//

#include "SSAOGraphicsPipeline.h"

#include <random>

#include "../storage/ImageTextureLoader.h"

#include "glm/ext.hpp"

SSAOGraphicsPipeline::SSAOGraphicsPipeline(ModelBufferGraphicsPipeline& pipeline): pipeline(pipeline), context(pipeline.getGraphicsPipeline().context), ubo_buffer(pipeline.getGraphicsPipeline().context) {
    ubo_buffer.allocateBuffer();
    createNoiseImage();
    createSamples();

    ubo.proj = glm::perspective(glm::radians(90.0), 2560.0 / 1440.0, 0.1, 100.0);
    ubo.view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0));
    ubo.view = glm::translate(ubo.view, glm::vec3(0, 0, -4));
}

void SSAOGraphicsPipeline::renderPipeline(Renderable::RenderArguments renderArguments) {
    pipeline.renderPipeline(renderArguments);
}

void SSAOGraphicsPipeline::prepareRender(Renderable::RenderArguments renderArguments) {
    pipeline.prepareRender(renderArguments);
    ubo_buffer.updateBuffer(ubo);
    ubo_buffer.writeToDescriptor(*descriptor_set);
    noise_sampler->updateSampler(*descriptor_set, 4);
    if (depth_sampler != nullptr) {
        depth_sampler->updateSampler(*descriptor_set, 3);
    }
}

void SSAOGraphicsPipeline::createNoiseImage() {
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    std::vector<glm::vec4> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec4 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f,
            0.0f);
        ssaoNoise.push_back(noise);
    }

    ImageTextureLoader imageLoader(context);
    noise_image = imageLoader.createImageFromBuffer(VK_FORMAT_R16G16B16A16_SFLOAT, 4, 4, ssaoNoise.data(), sizeof(ssaoNoise[0]) * ssaoNoise.size());

    vk::ImageViewCreateInfo image_view_create_info {};
    image_view_create_info.setComponents(vk::ComponentMapping());
    image_view_create_info.setFormat(vk::Format::eR16G16B16A16Sfloat);
    image_view_create_info.setImage(noise_image.image);
    image_view_create_info.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    image_view_create_info.setViewType(vk::ImageViewType::e2D);

    noise_image_view = context.device.createImageView(image_view_create_info);

    auto sampler = std::make_unique<CombinedDescriptorSampler> (context);
    sampler->targetImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    sampler->targetImageView = *noise_image_view;
    sampler->buildSampler();

    noise_sampler.swap(sampler);
}

GraphicsPipeline& SSAOGraphicsPipeline::getPipeline() {
    return pipeline.getGraphicsPipeline();
}

void SSAOGraphicsPipeline::destroy() {
    noise_image.destroy();
    ubo_buffer.destroy();
    pipeline.getGraphicsPipeline().destroy();
}

void SSAOGraphicsPipeline::createSamples() {
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator)
        );
        sample  = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = (float)i / 64.0;
        scale   = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ubo.samples[i] = sample;
    }
}

float SSAOGraphicsPipeline::lerp(float a, float b, float f) {
    return a + f * (b-a);
}

vk::ImageView SSAOGraphicsPipeline::getOcclusionImageView() {
    return *getPipeline().ownedImages[0].imageView;
}

vk::Image SSAOGraphicsPipeline::getOcclusionImage() {
    return getPipeline().ownedImages[0].imageAllocation.image;
}

