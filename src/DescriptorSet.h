//
// Created by Sherif Abdou on 8/4/23.
//

#ifndef VULKAN_ENGINE_DESCRIPTORSET_H
#define VULKAN_ENGINE_DESCRIPTORSET_H

#include "VulkanContext.h"

class DescriptorSet {
private:
    VulkanContext& context;
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
public:
    [[nodiscard]] const std::vector<vk::DescriptorSetLayoutBinding> &getBindings() const;

    void setBindings(const std::vector<vk::DescriptorSetLayoutBinding> &bindings);
private:
    vk::raii::DescriptorSet descriptorSet = nullptr;
public:
    [[nodiscard]] const vk::raii::DescriptorSet &getDescriptorSet() const;

    [[nodiscard]] const vk::raii::DescriptorSetLayout &getDescriptorSetLayout() const;

    [[nodiscard]] const vk::raii::DescriptorPool &getDescriptorPool() const;

private:
    vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
    vk::raii::DescriptorPool descriptorPool = nullptr;

    void createDescriptorLayout();
    void createDescriptorPool();
    void createDescriptorSet();
public:
    explicit DescriptorSet(VulkanContext &context);

    constexpr static unsigned int MAX_UNIFORM_BUFFERS = 8;
    constexpr static unsigned int MAX_SAMPLERS = 16;

    void buildDescriptor();
    void bindToCommandBuffer(vk::raii::CommandBuffer &, vk::raii::PipelineLayout& pipelineLayout, uint32_t set=0);
};


#endif //VULKAN_ENGINE_DESCRIPTORSET_H
