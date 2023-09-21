//
// Created by Sherif Abdou on 8/4/23.
//

#include "DescriptorSet.h"

DescriptorSet::DescriptorSet(VulkanContext &context) : context(context) {}

void DescriptorSet::createDescriptorLayout() {
    vk::DescriptorSetLayoutCreateInfo createInfo {};
    createInfo.setBindings(bindings);

    this->descriptorSetLayout = context.device.createDescriptorSetLayout(createInfo);
}

void DescriptorSet::createDescriptorPool() {
    vk::DescriptorPoolSize uniformBufferSize {};
    uniformBufferSize.setType(vk::DescriptorType::eUniformBuffer);
    uniformBufferSize.setDescriptorCount(MAX_UNIFORM_BUFFERS);

    vk::DescriptorPoolSize samplerSize {};
    samplerSize.setType(vk::DescriptorType::eCombinedImageSampler);
    samplerSize.setDescriptorCount(MAX_SAMPLERS);

    vk::DescriptorPoolCreateInfo createInfo {};
    auto sizes = std::vector<vk::DescriptorPoolSize> {uniformBufferSize, samplerSize};
    createInfo.setPoolSizes(sizes);
    createInfo.setMaxSets(1);

    descriptorPool = context.device.createDescriptorPool(createInfo);
}

void DescriptorSet::createDescriptorSet() {
    vk::DescriptorSetAllocateInfo allocateInfo {};

    auto list = std::vector<vk::DescriptorSetLayout> {*descriptorSetLayout};
    allocateInfo.setDescriptorPool(*descriptorPool);
    allocateInfo.setSetLayouts(list);

    descriptorSet = vk::raii::DescriptorSets (context.device, allocateInfo);
}

const std::vector<vk::DescriptorSetLayoutBinding> &DescriptorSet::getBindings() const {
    return bindings;
}

void DescriptorSet::setBindings(const std::vector<vk::DescriptorSetLayoutBinding> &bindings) {
    DescriptorSet::bindings = bindings;
}

vk::raii::DescriptorSet &DescriptorSet::getDescriptorSet() {
    return descriptorSet[0];
}

vk::raii::DescriptorSetLayout &DescriptorSet::getDescriptorSetLayout() {
    return descriptorSetLayout;
}

const vk::raii::DescriptorPool &DescriptorSet::getDescriptorPool() const {
    return descriptorPool;
}

void DescriptorSet::buildDescriptor() {
    createDescriptorLayout();
    createDescriptorPool();
    createDescriptorSet();
}

void
DescriptorSet::bindToCommandBuffer(vk::raii::CommandBuffer &commandBuffer, vk::raii::PipelineLayout& pipelineLayout, uint32_t set) {
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipelineLayout, set, *descriptorSet[0], dynamic_offsets);
}

DescriptorSet::DescriptorSet(VulkanContext &context, const std::vector<vk::DescriptorSetLayoutBinding> &bindings)
        : context(context), bindings(bindings) {}

DescriptorSet::~DescriptorSet() {
}

DescriptorSet DescriptorSet::duplicateWithSameLayout() {
    auto descriptorset = DescriptorSet(context);
    descriptorset.bindings = this->bindings;
    descriptorset.buildDescriptor();

    return descriptorset;
}
