//
// Created by Sherif Abdou on 12/4/23.
//

#include "DescriptorSampler.h"

DescriptorSampler::DescriptorSampler(VulkanContext& context): context(context) {
}

vk::raii::Sampler& DescriptorSampler::getSampler() {
    return this->sampler;
}

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

void DescriptorSampler::updateSampler(DescriptorSet& descriptorSet, unsigned int binding, unsigned int index) {
    vk::WriteDescriptorSet writeDescriptorSet {};
    writeDescriptorSet.setDstBinding(binding);
    writeDescriptorSet.setDstSet(descriptorSet.getDescriptorSet());
    writeDescriptorSet.setDescriptorCount(1);
    writeDescriptorSet.setDstArrayElement(index);
    writeDescriptorSet.setDescriptorType(vk::DescriptorType::eSampler);

    context.device.updateDescriptorSets(writeDescriptorSet, {});
}
