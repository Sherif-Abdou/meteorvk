#ifndef VULAKN_ENGINE_CUSTOMDESCRIPTORSET_H
#define VULAKN_ENGINE_CUSTOMDESCRIPTORSET_H

#include "core/storage/DescriptorSet.h"

// Subclass of DescriptorSet that allows for custom DescriptorPool allocation sizes
class CustomDescriptorSet: public DescriptorSet {
public:
    // Per frame maximums to allocate for each option
    struct AllocationOptions {
        uint32_t maxUniformBuffers;
        uint32_t maxDynamicUniformBuffers;
        uint32_t maxStorageBuffers;
        uint32_t maxDynamicStorageBuffers;
        uint32_t maxSampledImages;
        uint32_t maxSamplers;
        uint32_t maxCombinedSamplers;
    };

    AllocationOptions options;

    CustomDescriptorSet(VulkanContext* context, std::vector<vk::DescriptorSetLayoutBinding> bindings);

    void createDescriptorPool() override;

    void setAllocationOptionsFromBindings(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
};

#endif
