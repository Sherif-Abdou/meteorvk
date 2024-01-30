//
// Created by Sherif Abdou on 8/4/23.
//

#ifndef VULKAN_ENGINE_DESCRIPTORSET_H
#define VULKAN_ENGINE_DESCRIPTORSET_H

#include "../VulkanContext.h"

/// Wrapper around VkDescriptorSet
class DescriptorSet {
protected:
    unsigned int frames_in_flight = 2;
    unsigned int current_frame = 0;
    VulkanContext* context;
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    std::vector<vk::DescriptorSet> descriptorSet {};
    vk::DescriptorSetLayout descriptorSetLayout = nullptr;
    vk::DescriptorPool descriptorPool = nullptr;

    virtual void createDescriptorLayout();
    virtual void createDescriptorPool();
    virtual void createDescriptorSet();
public:
    [[nodiscard]] vk::DescriptorSet getDescriptorSet();

    [[nodiscard]] vk::DescriptorSetLayout getDescriptorSetLayout();

    explicit DescriptorSet(VulkanContext *context);
    DescriptorSet(DescriptorSet&&) = default;

    std::vector<unsigned int> dynamic_offsets = {};

    DescriptorSet(VulkanContext *context, const std::vector<vk::DescriptorSetLayoutBinding> &bindings);

    constexpr static unsigned int MAX_UNIFORM_BUFFERS = 8;
    constexpr static unsigned int MAX_STORAGE_BUFFERS = 8;
    constexpr static unsigned int MAX_SAMPLERS = 16;

    /// Builds the Vulkan Descriptor pool and sets
    void buildDescriptor();
    void bindToCommandBuffer(vk::raii::CommandBuffer &, vk::raii::PipelineLayout& pipelineLayout, uint32_t set=0);

    void bindToCommandBufferCompute(vk::raii::CommandBuffer&commandBuffer, vk::raii::PipelineLayout&pipelineLayout,
                                    uint32_t set=0);

    /// Moves to the next frame's DescriptorSet, used for multiple frames in flight
    void nextFrame();

    virtual ~DescriptorSet();
};


#endif //VULKAN_ENGINE_DESCRIPTORSET_H
