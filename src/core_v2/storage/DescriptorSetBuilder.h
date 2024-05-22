#ifndef VULKAN_ENGINE_DESCRIPTORSETBUILDER_H
#define VULKAN_ENGINE_DESCRIPTORSETBUILDER_H

#include <string>

#include "core/VulkanContext.h"
#include "core/storage/DescriptorSet.h"
#include "vulkan/vulkan_handles.hpp"
#include "vulkan/vulkan_raii.hpp"
#include "vulkan/vulkan_structs.hpp"

class DescriptorSetBuilder {
    public:
        struct DescriptorLayoutBinding {
            vk::DescriptorSetLayoutBinding layout;
            int32_t binding = -1;
        };
    private:
        VulkanContext* context;
        std::unordered_map<std::string, DescriptorLayoutBinding> layouts {};
        std::vector<std::string> layout_names {};

        std::optional<vk::raii::DescriptorSetLayout> built_layout;


        std::vector<DescriptorSet*> allocated_descriptors {};
    public:
        explicit DescriptorSetBuilder(VulkanContext* context);

        void addLayoutBinding(const std::string& name, vk::DescriptorSetLayoutBinding layout);
        void finalizeLayout();
        DescriptorSet* buildDescriptorSet();
        uint32_t getBindingOf(const std::string& name);

        vk::raii::DescriptorSetLayout* getDescriptorLayout();

        ~DescriptorSetBuilder();
};
#endif
