//
// Created by Sherif Abdou on 9/14/23.
//

#ifndef VULKAN_ENGINE_DESCRIPTORSAMPLER_H
#define VULKAN_ENGINE_DESCRIPTORSAMPLER_H

#include "../VulkanContext.h"
#include "DescriptorSet.h"

/// Wrapper around a sampler for a particular imageview
class DescriptorSampler {
private:
    VulkanContext& context;
    vk::raii::Sampler sampler = nullptr;
public:
    explicit DescriptorSampler(VulkanContext &context);

    const vk::raii::Sampler &getSampler() const;

    vk::ImageView targetImageView;
    void buildSampler();
    void updateSampler(DescriptorSet& descriptorSet, unsigned int binding);
};


#endif //VULKAN_ENGINE_DESCRIPTORSAMPLER_H
