//
// Created by Sherif Abdou on 5/5/24.
//

#ifndef VULKAN_ENGINE_NEWDESCRIPTORMANAGER_H
#define VULKAN_ENGINE_NEWDESCRIPTORMANAGER_H

#include <vulkan/vulkan.hpp>
#include <unordered_map>
#include "../core/VulkanContext.h"
#include "../core/storage/DescriptorSet.h"

class NewDescriptorManager {
    VulkanContext* context;
public:
    // How often a binding is expected to be rebound
    enum class BindingUpdateRate {
        Frame,
        Pipeline,
        Model,
    };
private:
    struct BindingLayoutInformation {
        vk::DescriptorSetLayoutBinding layout;
        BindingUpdateRate bind_rate;
        uint32_t set = -1;
        uint32_t binding = -1;
    };

    std::unordered_map<std::string, BindingLayoutInformation> layouts {};

    std::vector<std::string> layout_names {};
    std::vector<DescriptorSet*> descriptors {};
public:
    const std::vector<std::string> &getLayoutNames() const;

private:
    DescriptorSet* buildDescriptorsOfCategory(NewDescriptorManager::BindingUpdateRate rate, uint32_t set = 0);
public:
    explicit NewDescriptorManager(VulkanContext* context);
    // Adds a Descriptor Layout to a specific descriptor rate slot
    void addLayoutBinding(const std::string& name, vk::DescriptorSetLayoutBinding layout, BindingUpdateRate rate);


    std::vector<DescriptorSet*> buildDescriptors();

    uint32_t getBindingOf(const std::string& name);
    uint32_t getSetOf(const std::string& name);

    DescriptorSet* getDescriptorFor(const std::string & name);
    DescriptorSet* getDescriptorFor(BindingUpdateRate rate);

    ~NewDescriptorManager();
};


#endif //VULKAN_ENGINE_NEWDESCRIPTORMANAGER_H
