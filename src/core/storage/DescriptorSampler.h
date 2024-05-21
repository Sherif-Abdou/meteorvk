//
// Created by Sherif Abdou on 12/4/23.
//

#ifndef DESCRIPTORSAMPLER_H
#define DESCRIPTORSAMPLER_H

#include "DescriptorSet.h"
#include "core/VulkanContext.h"

class DescriptorSampler {
    VulkanContext* context;
    vk::raii::Sampler sampler = nullptr;
public:
    explicit DescriptorSampler(VulkanContext *context);

    vk::raii::Sampler &getSampler();

    void buildSampler();
    void updateSampler(DescriptorSet& descriptorSet, unsigned int binding, unsigned int index=0);
};



#endif //DESCRIPTORSAMPLER_H
