//
// Created by Sherif Abdou on 9/14/23.
//

#include "CombinedDescriptorSampler.h"

void CombinedDescriptorSampler::buildSampler() {
    vk::SamplerCreateInfo samplerCreateInfo {};
    samplerCreateInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
    samplerCreateInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
    samplerCreateInfo.setAnisotropyEnable(false);
    samplerCreateInfo.setMagFilter(vk::Filter::eNearest);
    samplerCreateInfo.setMinFilter(vk::Filter::eNearest);
    samplerCreateInfo.setMipmapMode(vk::SamplerMipmapMode::eNearest);

    sampler = context->device.createSampler(samplerCreateInfo);
}

void CombinedDescriptorSampler::updateSampler(DescriptorSet& descriptorSet, unsigned int binding) {
    vk::WriteDescriptorSet writeDescriptorSet {};
    writeDescriptorSet.setDstBinding(binding);
    writeDescriptorSet.setDstSet(descriptorSet.getDescriptorSet());
    writeDescriptorSet.setDescriptorCount(1);
    writeDescriptorSet.setDstArrayElement(0);
    writeDescriptorSet.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);

    vk::DescriptorImageInfo descriptorImageInfo {};
    descriptorImageInfo.setImageLayout(targetImageLayout);
    descriptorImageInfo.setImageView(targetImageView);
    descriptorImageInfo.setSampler(*sampler);

    writeDescriptorSet.setImageInfo(descriptorImageInfo);

    context->device.updateDescriptorSets(writeDescriptorSet, {});
}

const vk::raii::Sampler &CombinedDescriptorSampler::getSampler() const {
    return sampler;
}

CombinedDescriptorSampler::CombinedDescriptorSampler(VulkanContext *context) : context(context) {}
