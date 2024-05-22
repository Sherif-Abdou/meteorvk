//
// Created by Sherif Abdou on 5/5/24.
//

#include "NewDescriptorManager.h"
#include "core_v2/storage/DescriptorSetBuilder.h"

void NewDescriptorManager::addLayoutBinding(const std::string &name, vk::DescriptorSetLayoutBinding layout,
        NewDescriptorManager::BindingUpdateRate rate) {
    layout_names.push_back(name);
    if (rate == BindingUpdateRate::Frame) {
        perFrameDescriptorBuilder->addLayoutBinding(name, layout);
    }
}

uint32_t NewDescriptorManager::getBindingOf(const std::string &name) {
    return perFrameDescriptorBuilder->getBindingOf(name);
}

uint32_t NewDescriptorManager::getSetOf(const std::string &name) {
    if (perFrameDescriptorBuilder->getBindingOf(name) >= 0) {
        return 0;
    }
    return -1;
}

std::vector<DescriptorSet *> NewDescriptorManager::buildDescriptors() {
    if (!descriptors.empty()) {
        return descriptors;
    }
    descriptors = {};
    DescriptorSet* set;
    perFrameDescriptorBuilder->finalizeLayout();

    if ((perFrameDescriptor = perFrameDescriptorBuilder->buildDescriptorSet()) != nullptr) {
        descriptors.push_back(perFrameDescriptor);
    }

    return descriptors;
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

NewDescriptorManager::~NewDescriptorManager() {
}
