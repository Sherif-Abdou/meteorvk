//
// Created by Sherif Abdou on 12/4/23.
//

#include "StorageImage.h"

void StorageImage::updateDescriptor(DescriptorSet& descriptor, unsigned binding, unsigned index) {
    vk::WriteDescriptorSet writeDescriptorSet {};
    writeDescriptorSet.setDstBinding(binding);
    writeDescriptorSet.setDstSet(descriptor.getDescriptorSet());
    writeDescriptorSet.setDescriptorCount(1);
    writeDescriptorSet.setDstArrayElement(0);
    writeDescriptorSet.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);

    vk::DescriptorImageInfo descriptorImageInfo {};
    descriptorImageInfo.setImageLayout(target_image_layout);
    descriptorImageInfo.setImageView(target_image_view);

    writeDescriptorSet.setImageInfo(descriptorImageInfo);

    context.device.updateDescriptorSets(writeDescriptorSet, {});
}
