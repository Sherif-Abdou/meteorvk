//
// Created by Sherif Abdou on 8/4/23.
//

#ifndef VULKAN_ENGINE_DESCRIPTORSET_H
#define VULKAN_ENGINE_DESCRIPTORSET_H

#include "../VulkanContext.h"

class DescriptorSet {
private:
    static constexpr unsigned int FRAMES_IN_FLIGHT = 2;
    unsigned int current_frame = 0;
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

    std::vector<unsigned int> dynamic_offsets = {};

    DescriptorSet(VulkanContext &context, const std::vector<vk::DescriptorSetLayoutBinding> &bindings);

    DescriptorSet duplicateWithSameLayout();

    constexpr static unsigned int MAX_UNIFORM_BUFFERS = 8;
    constexpr static unsigned int MAX_SAMPLERS = 16;

    void buildDescriptor();
    void bindToCommandBuffer(vk::raii::CommandBuffer &, vk::raii::PipelineLayout& pipelineLayout, uint32_t set=0);

    void nextFrame();

    virtual ~DescriptorSet();
};


#endif //VULKAN_ENGINE_DESCRIPTORSET_H
