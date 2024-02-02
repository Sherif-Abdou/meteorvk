//
// Created by Sherif Abdou on 1/30/24.
//

#include "TextureContainer.h"

void TextureContainer::destroy() {
    for (auto& texture: textures) {
        texture.allocation.destroy();
        delete texture.storageImage;
    }

    for (auto sampler: samplers) {
        delete sampler;
    }
}

uint32_t TextureContainer::addTexture(VulkanAllocator::VulkanImageAllocation allocation, vk::raii::ImageView imageView,
                                      StorageImage* storageImage) {
    uint32_t i = textures.size();

    textures.push_back({allocation, std::move(imageView), storageImage});

    return i;
}

uint32_t TextureContainer::addTexture(TextureContainer::Texture texture) {
    uint32_t i = textures.size();

    textures.push_back(std::move(texture));

    return i;
}

StorageImage *TextureContainer::getStorageImage(uint32_t index) {
    return textures[index].storageImage;
}

vk::raii::ImageView *TextureContainer::getImageView(uint32_t index) {
    return &textures[index].imageView;
}

VulkanAllocator::VulkanImageAllocation *TextureContainer::getAllocation(uint32_t index) {
    return &textures[index].allocation;
}

TextureContainer::Texture& TextureContainer::operator[](uint32_t index) {
    return textures[index];
}

uint32_t TextureContainer::getTextureSize() {
    return textures.size();
}

uint32_t TextureContainer::getSamplerSize() {
    return samplers.size();
}

uint32_t TextureContainer::addSampler(DescriptorSampler *sampler) {
    auto i = samplers.size();
    samplers.push_back(sampler);
    return i;
}
