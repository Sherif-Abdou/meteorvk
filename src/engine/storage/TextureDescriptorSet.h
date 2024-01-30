//
// Created by Sherif Abdou on 1/28/24.
//

#ifndef VULKAN_ENGINE_TEXTUREDESCRIPTORSET_H
#define VULKAN_ENGINE_TEXTUREDESCRIPTORSET_H


#include "../../core/storage/DescriptorSet.h"

class TextureDescriptorSet: public DescriptorSet {
protected:
    void createDescriptorPool() override;

    void createDescriptorSet() override;

    void createDescriptorLayout() override;

public:
    explicit TextureDescriptorSet(VulkanContext* context);
    const uint32_t max_images = 100;
    const uint32_t max_samplers = 8;
};


#endif //VULKAN_ENGINE_TEXTUREDESCRIPTORSET_H
