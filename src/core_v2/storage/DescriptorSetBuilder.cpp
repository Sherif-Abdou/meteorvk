#include "DescriptorSetBuilder.h"
#include "core/VulkanContext.h"
#include "core/storage/DescriptorSet.h"
#include "core_v2/storage/CustomDescriptorSet.h"
#include "vulkan/vulkan_structs.hpp"

DescriptorSetBuilder::DescriptorSetBuilder(VulkanContext* context): context(context) {

}

void DescriptorSetBuilder::addLayoutBinding(const std::string& name, vk::DescriptorSetLayoutBinding layout) {
    layout_names.push_back(name);
    layouts[name] = {
        layout,
        -1
    };
}

void DescriptorSetBuilder::finalizeLayout() {
    if (built_layout.has_value()) {
        return;
    }
    int i = 0;
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    for (const auto& name: layout_names) {
        layouts[name].layout.setBinding(i);
        layouts[name].binding = i;
        bindings.push_back(layouts[name].layout);
        i+=1;
    }

    vk::DescriptorSetLayoutCreateInfo createInfo {};
    createInfo.setBindings(bindings);

    built_layout = context->device.createDescriptorSetLayout(createInfo);
}

DescriptorSet* DescriptorSetBuilder::buildDescriptorSet() {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;

    for (const auto& name: layout_names) {
        bindings.push_back(layouts[name].layout);
    }
    CustomDescriptorSet* descriptor = new CustomDescriptorSet(context, bindings);
    descriptor->setAllocationOptionsFromBindings(bindings);
    descriptor->buildDescriptor();

    allocated_descriptors.push_back(descriptor);

    return descriptor;
};

vk::raii::DescriptorSetLayout* DescriptorSetBuilder::getDescriptorLayout() {
    if (!built_layout.has_value()) {
        return nullptr;
    }

    return &(built_layout.value());
}

DescriptorSetBuilder::~DescriptorSetBuilder() {
    for (auto descriptor: allocated_descriptors) {
        delete descriptor;
    }
}


int32_t DescriptorSetBuilder::getBindingOf(const std::string& name) {
    if (!this->layouts.contains(name)) {
        return -1;
    }

    return layouts[name].binding;
}

void DescriptorSetBuilder::tickAllDescriptors() {
    for (auto descriptor: allocated_descriptors) {
        if (descriptor != nullptr) {
            descriptor->nextFrame();
        }
    }
}
