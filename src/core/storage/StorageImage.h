//
// Created by Sherif Abdou on 12/4/23.
//

#ifndef STORAGEIMAGE_H
#define STORAGEIMAGE_H

#include "DescriptorSet.h"
#include "core/VulkanContext.h"

class StorageImage {
    VulkanContext* context;
public:
    explicit StorageImage(VulkanContext* context): context(context) {};

    void updateDescriptor(DescriptorSet& descriptor, unsigned int binding, unsigned int index=0);

    vk::ImageView target_image_view;

    void setTargetImageView(const vk::ImageView &targetImageView);

    void setTargetImageLayout(vk::ImageLayout targetImageLayout);

    vk::ImageLayout target_image_layout;
};



#endif //STORAGEIMAGE_H
