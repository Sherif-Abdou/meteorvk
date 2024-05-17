//
// Created by Sherif Abodu on 4/29/24
//
#ifndef VULKAN_ENGINE_NEWDESCRIPTORMANAGER_H
#define VULKAN_ENGINE_DESCRIPTORMANAGER_H

#include <vector>
#include <unordered_map>
#include <string>
#include "../../core/storage/DescriptorSet.h"

class DescriptorManager {
    std::unordered_map<std::string, DescriptorSet*> descriptors {}; 
    VulkanContext* context;

  public:
    explicit DescriptorManager(VulkanContext* context);

    /// Adds a descriptor to the internal map
    void addDescriptorSet(const std::string& name, DescriptorSet* set);

    /// Returns the descriptor set with given name.
    /// Returns a nullptr if the name doesn't exist
    DescriptorSet* getDescriptorSet(const std::string& name);

    /// Checks if the NewDescriptorManager contains every name in a vector
    bool containsMandatoryDescriptors(const std::vector<std::string>& name_list);
};

#endif
