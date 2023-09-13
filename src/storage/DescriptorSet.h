//
// Created by Sherif Abdou on 8/4/23.
//

#ifndef VULKAN_ENGINE_DESCRIPTORSET_H
#define VULKAN_ENGINE_DESCRIPTORSET_H

#include "../VulkanContext.h"

class DescriptorSet {
private:
    VulkanContext& context;
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    std::vector<vk::raii::DescriptorSet> descriptorSet {};
    vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
    vk::raii::DescriptorPool descriptorPool = nullptr;

    void createDescriptorLayout();
    void createDescriptorPool();
    void createDescriptorSet();
public:
    [[nodiscard]] const std::vector<vk::DescriptorSetLayoutBinding> &getBindings() const;

    void setBindings(const std::vector<vk::DescriptorSetLayoutBinding> &bindings);
    [[nodiscard]] vk::raii::DescriptorSet &getDescriptorSet();

    [[nodiscard]] vk::raii::DescriptorSetLayout &getDescriptorSetLayout();

    [[nodiscard]] const vk::raii::DescriptorPool &getDescriptorPool() const;
    explicit DescriptorSet(VulkanContext &context);
    DescriptorSet(DescriptorSet&&) = default;

    DescriptorSet(VulkanContext &context, const std::vector<vk::DescriptorSetLayoutBinding> &bindings);

    constexpr static unsigned int MAX_UNIFORM_BUFFERS = 8;
    constexpr static unsigned int MAX_SAMPLERS = 16;

    void buildDescriptor();
    void bindToCommandBuffer(vk::raii::CommandBuffer &, vk::raii::PipelineLayout& pipelineLayout, uint32_t set=0);

    virtual ~DescriptorSet();
};


#endif //VULKAN_ENGINE_DESCRIPTORSET_H
