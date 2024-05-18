//
// Created by Sherif Abdou on 8/4/23.
//

#include "DescriptorSet.h"

DescriptorSet::DescriptorSet(VulkanContext *context) : context(context) {}

void DescriptorSet::createDescriptorLayout() {
    vk::DescriptorSetLayoutCreateInfo createInfo {};
    createInfo.setBindings(bindings);

    for (auto& binding: bindings) {
        if (binding.descriptorType == vk::DescriptorType::eUniformBufferDynamic || binding.descriptorType == vk::DescriptorType::eStorageBufferDynamic) {
            dynamic_offsets.push_back(0);
        }
    }

    this->descriptorSetLayout = context->device.createDescriptorSetLayout(createInfo).release();
}

void DescriptorSet::createDescriptorPool() {
    vk::DescriptorPoolSize uniformBufferSize {};
    uniformBufferSize.setType(vk::DescriptorType::eUniformBuffer);
    uniformBufferSize.setDescriptorCount(MAX_UNIFORM_BUFFERS);

    vk::DescriptorPoolSize samplerSize {};
    samplerSize.setType(vk::DescriptorType::eCombinedImageSampler);
    samplerSize.setDescriptorCount(MAX_SAMPLERS);

    vk::DescriptorPoolSize storageBufferSize {};
    storageBufferSize.setType(vk::DescriptorType::eStorageBuffer);
    storageBufferSize.setDescriptorCount(MAX_STORAGE_BUFFERS);

    vk::DescriptorPoolSize dynamicUniformBufferSize {};
    dynamicUniformBufferSize.setType(vk::DescriptorType::eUniformBufferDynamic);
    dynamicUniformBufferSize.setDescriptorCount(2);

    vk::DescriptorPoolCreateInfo createInfo {};
    auto sizes = std::vector<vk::DescriptorPoolSize> {uniformBufferSize, samplerSize, dynamicUniformBufferSize, storageBufferSize};
    createInfo.setPoolSizes(sizes);
    createInfo.setMaxSets(frames_in_flight);

    descriptorPool = context->device.createDescriptorPool(createInfo).release();
}

void DescriptorSet::createDescriptorSet() {
    vk::DescriptorSetAllocateInfo allocateInfo {};

    auto list = std::vector<vk::DescriptorSetLayout> {descriptorSetLayout};
    list.push_back(descriptorSetLayout);
    allocateInfo.setDescriptorPool(descriptorPool);
    allocateInfo.setSetLayouts(list);

    descriptorSet = (*context->device).allocateDescriptorSets(allocateInfo);
}

vk::DescriptorSet DescriptorSet::getDescriptorSet() {
    return descriptorSet[current_frame];
}

vk::DescriptorSetLayout DescriptorSet::getDescriptorSetLayout() {
    return descriptorSetLayout;
}
void DescriptorSet::buildDescriptor() {
    createDescriptorLayout();
    createDescriptorPool();
    createDescriptorSet();
}

void
DescriptorSet::bindToCommandBuffer(vk::raii::CommandBuffer *commandBuffer, vk::raii::PipelineLayout& pipelineLayout, uint32_t set) {
    commandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipelineLayout, set, descriptorSet[current_frame], dynamic_offsets);
}

void
DescriptorSet::bindToCommandBufferCompute(vk::raii::CommandBuffer &commandBuffer, vk::raii::PipelineLayout& pipelineLayout, uint32_t set) {
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, *pipelineLayout, set, descriptorSet[current_frame], dynamic_offsets);
}

DescriptorSet::DescriptorSet(VulkanContext *context, const std::vector<vk::DescriptorSetLayoutBinding> &bindings)
        : context(context), bindings(bindings) {}

DescriptorSet::~DescriptorSet() {
    (*context->device).destroy(descriptorSetLayout);
    (*context->device).destroy(descriptorPool);
}

void DescriptorSet::nextFrame() {
    current_frame = (current_frame + 1) % frames_in_flight;
}
