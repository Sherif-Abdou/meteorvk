#include "CustomDescriptorSet.h"

void CustomDescriptorSet::createDescriptorPool() {
    std::vector<vk::DescriptorPoolSize> sizes = {};

    vk::DescriptorPoolSize uniformBufferSize {};
    uniformBufferSize.setType(vk::DescriptorType::eUniformBuffer);
    uniformBufferSize.setDescriptorCount(options.maxUniformBuffers * frames_in_flight);
    if (uniformBufferSize.descriptorCount > 0) {
        sizes.push_back(uniformBufferSize);
    }

    vk::DescriptorPoolSize combinedSamplerSize {};
    combinedSamplerSize.setType(vk::DescriptorType::eCombinedImageSampler);
    combinedSamplerSize.setDescriptorCount(options.maxCombinedSamplers * frames_in_flight);
    if (combinedSamplerSize.descriptorCount > 0) {
        sizes.push_back(combinedSamplerSize);
    }

    vk::DescriptorPoolSize storageBufferSize {};
    storageBufferSize.setType(vk::DescriptorType::eStorageBuffer);
    storageBufferSize.setDescriptorCount(options.maxStorageBuffers * frames_in_flight);
    if (storageBufferSize.descriptorCount > 0) {
        sizes.push_back(storageBufferSize);
    }

    vk::DescriptorPoolSize dynamicUniformBufferSize {};
    dynamicUniformBufferSize.setType(vk::DescriptorType::eUniformBufferDynamic);
    dynamicUniformBufferSize.setDescriptorCount(options.maxDynamicUniformBuffers * frames_in_flight);
    if (dynamicUniformBufferSize.descriptorCount > 0) {
        sizes.push_back(dynamicUniformBufferSize);
    }

    vk::DescriptorPoolSize samplerSize {};
    samplerSize.setType(vk::DescriptorType::eSampler);
    samplerSize.setDescriptorCount(options.maxSamplers * frames_in_flight);
    if (samplerSize.descriptorCount > 0) {
        sizes.push_back(samplerSize);
    }

    vk::DescriptorPoolSize sampledImageSize {};
    sampledImageSize.setType(vk::DescriptorType::eSampledImage);
    sampledImageSize.setDescriptorCount(options.maxSampledImages * frames_in_flight);
    if (sampledImageSize.descriptorCount > 0) {
        sizes.push_back(sampledImageSize);
    }

    vk::DescriptorPoolCreateInfo createInfo {};
    createInfo.setPoolSizes(sizes);
    createInfo.setMaxSets(frames_in_flight);

    descriptorPool = context->device.createDescriptorPool(createInfo).release();
}

void CustomDescriptorSet::setAllocationOptionsFromBindings(const std::vector<vk::DescriptorSetLayoutBinding>& bindings) {
    options = {0,0,0,0,0,0,0,};

    for (const auto& binding : bindings) {
        switch (binding.descriptorType) {
            case vk::DescriptorType::eSampledImage:
                options.maxSampledImages += binding.descriptorCount * frames_in_flight;
                break;
            case vk::DescriptorType::eCombinedImageSampler:
                options.maxCombinedSamplers += binding.descriptorCount * frames_in_flight;
                break;
            case vk::DescriptorType::eSampler:
                options.maxSamplers += binding.descriptorCount * frames_in_flight;
                break;
            case vk::DescriptorType::eUniformBuffer:
                options.maxUniformBuffers += binding.descriptorCount * frames_in_flight;
                break;
            case vk::DescriptorType::eUniformBufferDynamic:
                options.maxDynamicUniformBuffers += binding.descriptorCount * frames_in_flight;
                break;
            case vk::DescriptorType::eStorageBuffer:
                options.maxStorageBuffers += binding.descriptorCount * frames_in_flight;
                break;
            case vk::DescriptorType::eStorageBufferDynamic:
                options.maxDynamicUniformBuffers += binding.descriptorCount * frames_in_flight;
                break;
            default:
                break;
        }
    }
}


CustomDescriptorSet::CustomDescriptorSet(VulkanContext* context, std::vector<vk::DescriptorSetLayoutBinding> bindings)
    : DescriptorSet(context, bindings) {
}
