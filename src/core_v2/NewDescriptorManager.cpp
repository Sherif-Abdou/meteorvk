//
// Created by Sherif Abdou on 5/5/24.
//

#include "NewDescriptorManager.h"
#include "core_v2/storage/DescriptorSetBuilder.h"

void NewDescriptorManager::addLayoutBindingForFrame(const std::string &name, vk::DescriptorSetLayoutBinding layout) {
    layout_names.push_back(name);
    perFrameDescriptorBuilder->addLayoutBinding(name, layout);
}

uint32_t NewDescriptorManager::getBindingOf(const std::string &name, const std::string &pipeline_name) {
    int32_t binding = -1;
    if (perPipelineDescriptorBuilders.contains(pipeline_name) && ((binding = perPipelineDescriptorBuilders[pipeline_name]->getBindingOf(name)) >= 0)) {
        return binding;
    }
    if ((binding = perModelDescriptorBuilder->getBindingOf(name)) >= 0) {
        return binding;
    }
    if ((binding = perFrameDescriptorBuilder->getBindingOf(name)) >= 0) {
        return binding;
    }
    return binding;
}

uint32_t NewDescriptorManager::getSetOf(const std::string &name, const std::string &pipeline_name) {
    if (perPipelineDescriptorBuilders.contains(pipeline_name) && (perPipelineDescriptorBuilders[pipeline_name]->getBindingOf(name) >= 0)) {
        return 2;
    }
    if (perModelDescriptorBuilder != nullptr && (perModelDescriptorBuilder->getBindingOf(name) >= 0)) {
        return 1;
    }
    if (perFrameDescriptorBuilder != nullptr && perFrameDescriptorBuilder->getBindingOf(name) >= 0) {
        return 0;
    }
    return -1;
}

std::vector<vk::raii::DescriptorSetLayout*> NewDescriptorManager::getDescriptorLayoutsForPipeline(const std::string& pipeline_name) {
    std::vector<vk::raii::DescriptorSetLayout*> descriptor_layouts {};

    if (this->perFrameDescriptorBuilder != nullptr) {
        perFrameDescriptorBuilder->finalizeLayout();
        descriptor_layouts.push_back(perFrameDescriptorBuilder->getDescriptorLayout());
    }
    if (this->perModelDescriptorBuilder != nullptr) {
        perModelDescriptorBuilder->finalizeLayout();
        descriptor_layouts.push_back(perModelDescriptorBuilder->getDescriptorLayout());
    }
    if (this->perPipelineDescriptorBuilders.contains(pipeline_name)) {
        this->perPipelineDescriptorBuilders[pipeline_name]->finalizeLayout();
        descriptor_layouts.push_back(this->perPipelineDescriptorBuilders[pipeline_name]->getDescriptorLayout());
    }     

    return descriptor_layouts;
}
std::vector<vk::raii::DescriptorSetLayout*> NewDescriptorManager::buildDescriptorsForPipeline(const std::string& pipeline_name) {
    if (!descriptors.empty()) {
        return getDescriptorLayoutsForPipeline(pipeline_name);
    }
    descriptors = {};
    DescriptorSet* set;
    perFrameDescriptorBuilder->finalizeLayout();

    if (perFrameDescriptorBuilder != nullptr && ((perFrameDescriptor = perFrameDescriptorBuilder->buildDescriptorSet()) != nullptr)) {
        descriptors.push_back(perFrameDescriptor);
    }

    return getDescriptorLayoutsForPipeline(pipeline_name);
}

DescriptorSet* NewDescriptorManager::buildModelDescriptorSet() {
    perModelDescriptorBuilder->finalizeLayout();
    return perModelDescriptorBuilder->buildDescriptorSet();
}

DescriptorSet *NewDescriptorManager::buildDescriptorsOfCategory(NewDescriptorManager::BindingUpdateRate rate, uint32_t set) {
    if (rate == BindingUpdateRate::Frame) {
        perFrameDescriptor = perFrameDescriptorBuilder->buildDescriptorSet();

        return perFrameDescriptor;
    }
    return nullptr;
}

const std::vector<std::string> &NewDescriptorManager::getLayoutNames() const {
    return layout_names;
}

NewDescriptorManager::NewDescriptorManager(VulkanContext *context): context(context) {
    perFrameDescriptorBuilder = std::make_unique<DescriptorSetBuilder>(context);
}

DescriptorSet *NewDescriptorManager::getDescriptorFor(const std::string &name) {
    if (perFrameDescriptorBuilder->getBindingOf(name) >= 0) {
        return perFrameDescriptor;
    }
    return nullptr;
}

DescriptorSet* NewDescriptorManager::getDescriptorFor(NewDescriptorManager::BindingUpdateRate rate) {
    switch (rate) {
        case NewDescriptorManager::BindingUpdateRate::Frame:
            return this->descriptors[0];
        case NewDescriptorManager::BindingUpdateRate::Pipeline:
            return this->descriptors[1];
        case NewDescriptorManager::BindingUpdateRate::Model:
            return this->descriptors[2];
    }
}

void NewDescriptorManager::addLayoutBindingForPipeline(const std::string& name, const std::string& pipeline_name, vk::DescriptorSetLayoutBinding binding) {
    if (!perPipelineDescriptorBuilders.contains(pipeline_name)) {
        perPipelineDescriptorBuilders[pipeline_name] = std::make_unique<DescriptorSetBuilder>(context);
    }
    perPipelineDescriptorBuilders[pipeline_name]->addLayoutBinding(name, binding);
    layout_names.push_back(name);
};
void NewDescriptorManager::addModelLayoutBinding(const std::string& name,vk::DescriptorSetLayoutBinding binding) {
    layout_names.push_back(name);
    if (perModelDescriptorBuilder == nullptr) {
        perModelDescriptorBuilder = std::make_unique<DescriptorSetBuilder>(context);
    }
    perModelDescriptorBuilder->addLayoutBinding(name, binding);
}

void NewDescriptorManager::tickAllDescriptors() {
    if (perFrameDescriptorBuilder != nullptr) {
        perFrameDescriptorBuilder->tickAllDescriptors();
    }
    if (perModelDescriptorBuilder != nullptr) {
        perModelDescriptorBuilder->tickAllDescriptors();
    }
    for (auto& [_, v] : perPipelineDescriptorBuilders) {
        if (v != nullptr) {
            v->tickAllDescriptors();
        }
    }
}

NewDescriptorManager::~NewDescriptorManager() {
}

DescriptorSet* NewDescriptorManager::buildPipelineDescriptorSet(const std::string& name) {
    if (!perPipelineDescriptorBuilders.contains(name)) {
        return nullptr;
    }
    return perPipelineDescriptorBuilders[name]->buildDescriptorSet();
}
