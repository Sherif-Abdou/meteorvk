//
// Created by Sherif Abdou on 5/5/24.
//

#ifndef VULKAN_ENGINE_NEWDESCRIPTORMANAGER_H
#define VULKAN_ENGINE_NEWDESCRIPTORMANAGER_H

#include <memory>
#include <vulkan/vulkan.hpp>
#include <unordered_map>
#include "core/VulkanContext.h"
#include "core/storage/DescriptorSet.h"
#include "core_v2/storage/DescriptorSetBuilder.h"
#include "storage/CustomDescriptorSet.h"

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

    // Each pipeline might need a different layout of per pipeline information
    std::unordered_map<std::string, std::unique_ptr<DescriptorSetBuilder>> perPipelineDescriptorBuilders;

    // I don't want different models to have different descriptor layouts, they can have different individual descriptors though
    std::unique_ptr<DescriptorSetBuilder> perModelDescriptorBuilder = nullptr;

    std::unique_ptr<DescriptorSetBuilder> perFrameDescriptorBuilder = nullptr;
    DescriptorSet* perFrameDescriptor = nullptr;

    std::vector<std::string> layout_names {};
    std::vector<DescriptorSet*> descriptors {};
public:
    const std::vector<std::string> &getLayoutNames() const;

private:
    DescriptorSet* buildDescriptorsOfCategory(NewDescriptorManager::BindingUpdateRate rate, uint32_t set = 0);
public:
    explicit NewDescriptorManager(VulkanContext* context);
    // Adds a Descriptor Layout Binding for the global per frame descriptor
    void addLayoutBindingForFrame(const std::string& name, vk::DescriptorSetLayoutBinding layout);
    void addLayoutBindingForPipeline(const std::string& name, const std::string& pipeline_name, vk::DescriptorSetLayoutBinding binding);
    void addModelLayoutBinding(const std::string& name,vk::DescriptorSetLayoutBinding binding);

    DescriptorSet* buildModelDescriptorSet();
    DescriptorSet* buildPipelineDescriptorSet(const std::string& name);

    std::vector<vk::raii::DescriptorSetLayout*> buildDescriptorsForPipeline(const std::string& pipeline_name);

    std::vector<vk::raii::DescriptorSetLayout*> getDescriptorLayoutsForPipeline(const std::string& pipeline_name);

    uint32_t getBindingOf(const std::string& name, const std::string& pipeline_name = "");
    uint32_t getSetOf(const std::string& name, const std::string& pipeline_name = "");

    DescriptorSet* getDescriptorFor(const std::string & name);
    DescriptorSet* getDescriptorFor(BindingUpdateRate rate);

    void tickAllDescriptors();

    ~NewDescriptorManager();
};


#endif //VULKAN_ENGINE_NEWDESCRIPTORMANAGER_H
