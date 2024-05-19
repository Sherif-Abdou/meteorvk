//
// Created by Sherif Abdou on 5/5/24.
//

#include "NewDescriptorManager.h"

void NewDescriptorManager::addLayoutBinding(const std::string &name, vk::DescriptorSetLayoutBinding layout,
        NewDescriptorManager::BindingUpdateRate rate) {
    BindingLayoutInformation layout_info;
    layout_info.layout = layout;
    layout_info.bind_rate = rate;

    layout_names.push_back(name);

    layouts[name] = layout_info;
}

uint32_t NewDescriptorManager::getBindingOf(const std::string &name) {
    if (layouts.find(name) == layouts.end()) {
        return -1;
    }
    return layouts[name].binding;
}

uint32_t NewDescriptorManager::getSetOf(const std::string &name) {
    if (layouts.find(name) == layouts.end()) {
        return -1;
    }
    return layouts[name].set;
}

std::vector<DescriptorSet *> NewDescriptorManager::buildDescriptors() {
    if (!descriptors.empty()) {
        return descriptors;
    }
    descriptors = {};
    DescriptorSet* set;
    if ((set = buildDescriptorsOfCategory(BindingUpdateRate::Frame, 0)) != nullptr) {
        descriptors.push_back(set);
    }
    if ((set = buildDescriptorsOfCategory(BindingUpdateRate::Pipeline, 1)) != nullptr) {
        descriptors.push_back(set);
    }
    if ((set = buildDescriptorsOfCategory(BindingUpdateRate::Model, 2)) != nullptr) {
        descriptors.push_back(set);
    }

    return descriptors;
}

DescriptorSet *NewDescriptorManager::buildDescriptorsOfCategory(NewDescriptorManager::BindingUpdateRate rate, uint32_t set) {
    uint32_t current_binding = 0;
    std::vector<vk::DescriptorSetLayoutBinding> bindings {};
    for (const auto& [name, _] : layouts) {
        auto& layout = layouts[name];
        if (layout.bind_rate != rate) { continue; }
        layout.layout.setBinding(current_binding);
        layout.set = set;
        layout.binding = current_binding;
        current_binding += 1;
        bindings.push_back(layout.layout);
    }
    if (bindings.empty()) {
        return nullptr;
    }
    auto res = new DescriptorSet(context, bindings);

    res->buildDescriptor();

    return res;
}

const std::vector<std::string> &NewDescriptorManager::getLayoutNames() const {
    return layout_names;
}

NewDescriptorManager::NewDescriptorManager(VulkanContext *context): context(context) {}

DescriptorSet *NewDescriptorManager::getDescriptorFor(const std::string &name) {
    if (descriptors.empty() || (layouts.find(name) == layouts.end())) {
        return nullptr;
    }
    auto set = getSetOf(name);
    return descriptors[set];
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
    for (auto descriptor: descriptors) {
        delete descriptor;
    }
}
