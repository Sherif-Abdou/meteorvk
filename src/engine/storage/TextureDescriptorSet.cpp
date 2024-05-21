//
// Created by Sherif Abdou on 1/28/24.
//

#include "TextureDescriptorSet.h"

TextureDescriptorSet::TextureDescriptorSet(VulkanContext *context): material_storage_buffer(context) {
    material_storage_buffer.allocateBuffer();
}

void TextureDescriptorSet::uploadMaterialList() {
    material_storage_buffer.updateBuffer(materialList);
    auto* descriptor = descriptorManager->getDescriptorFor(MATERIAL_BUFFER_NAME);
    material_storage_buffer.writeToDescriptor(*descriptor, descriptorManager->getBindingOf(MATERIAL_BUFFER_NAME));
}

TextureDescriptorSet::~TextureDescriptorSet() {
    material_storage_buffer.destroy();
}

void TextureDescriptorSet::attachLayoutToDescriptorManager(NewDescriptorManager* descriptor) {
    descriptor->addLayoutBinding(MATERIAL_BUFFER_NAME, vk::DescriptorSetLayoutBinding()
            .setStageFlags(vk::ShaderStageFlagBits::eAll)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eStorageBuffer), 
            NewDescriptorManager::BindingUpdateRate::Frame);
}
