//
// Created by Sherif Abdou on 12/4/23.
//

#include "StorageImage.h"

void StorageImage::updateDescriptor(DescriptorSet& descriptor, unsigned binding, unsigned index) {
    vk::WriteDescriptorSet writeDescriptorSet {};
    writeDescriptorSet.setDstBinding(binding);
    writeDescriptorSet.setDstSet(descriptor.getDescriptorSet());
    writeDescriptorSet.setDescriptorCount(1);
    writeDescriptorSet.setDstArrayElement(index);
    writeDescriptorSet.setDescriptorType(vk::DescriptorType::eSampledImage);

    vk::DescriptorImageInfo descriptorImageInfo {};
    descriptorImageInfo.setImageLayout(target_image_layout);
    descriptorImageInfo.setImageView(target_image_view);

    writeDescriptorSet.setImageInfo(descriptorImageInfo);

    context->device.updateDescriptorSets(writeDescriptorSet, {});
}

void StorageImage::setTargetImageView(const vk::ImageView &targetImageView) {
    target_image_view = targetImageView;
}

void StorageImage::setTargetImageLayout(vk::ImageLayout targetImageLayout) {
    target_image_layout = targetImageLayout;
}
