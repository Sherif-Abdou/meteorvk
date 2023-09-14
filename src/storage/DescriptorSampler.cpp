//
// Created by Sherif Abdou on 9/14/23.
//

#include "DescriptorSampler.h"

void DescriptorSampler::buildSampler() {
    vk::SamplerCreateInfo samplerCreateInfo {};
    samplerCreateInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
    samplerCreateInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
    samplerCreateInfo.setAnisotropyEnable(false);
    samplerCreateInfo.setMagFilter(vk::Filter::eNearest);
    samplerCreateInfo.setMinFilter(vk::Filter::eNearest);
    samplerCreateInfo.setMipmapMode(vk::SamplerMipmapMode::eNearest);

    sampler = context.device.createSampler(samplerCreateInfo);
}

void DescriptorSampler::updateSampler(DescriptorSet& descriptorSet, unsigned int binding) {
    vk::WriteDescriptorSet writeDescriptorSet {};
    writeDescriptorSet.setDstBinding(binding);
    writeDescriptorSet.setDstSet(*descriptorSet.getDescriptorSet());
    writeDescriptorSet.setDescriptorCount(1);
    writeDescriptorSet.setDstArrayElement(0);
    writeDescriptorSet.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);

    vk::DescriptorImageInfo descriptorImageInfo {};
    descriptorImageInfo.setImageLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);
    descriptorImageInfo.setImageView(targetImageView);
    descriptorImageInfo.setSampler(*sampler);

    writeDescriptorSet.setImageInfo(descriptorImageInfo);

    context.device.updateDescriptorSets(writeDescriptorSet, {});
}

const vk::raii::Sampler &DescriptorSampler::getSampler() const {
    return sampler;
}

DescriptorSampler::DescriptorSampler(VulkanContext &context) : context(context) {}
