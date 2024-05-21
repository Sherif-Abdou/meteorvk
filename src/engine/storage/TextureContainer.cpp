//
// Created by Sherif Abdou on 1/30/24.
//

#include "TextureContainer.h"

#include "ImageTextureLoader.h"


TextureContainer::TextureContainer(VulkanContext* context): context(context) {

}

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

uint32_t TextureContainer::addTextureFromPath(const char* path) {
    assert(context != nullptr);

    ImageTextureLoader loader(context);
    auto textureImage = loader.loadImageFromFile(path);

    vk::ImageViewCreateInfo createInfo {};
    createInfo.setImage(textureImage.image);
    createInfo.setComponents(vk::ComponentMapping());
    createInfo.setFormat(vk::Format::eR8G8B8A8Srgb);
    createInfo.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    createInfo.setViewType(vk::ImageViewType::e2D);

    auto textureImageView = context->device.createImageView(createInfo);


    auto* image = new StorageImage(context);
    image->setTargetImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    image->setTargetImageView(*textureImageView);

    return addTexture(textureImage, std::move(textureImageView), image);
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

uint32_t TextureContainer::addMaterial(RenderMaterial material) {
    auto i = materials.size();
    materials.push_back(material);
    return i;
}

void TextureContainer::copyMaterialsTo(TextureDescriptorSet *set) {
    int index_slot = -1;
    for (int i = 0; i < materials.size(); i++) {
        set->materialList.materials[i] = materials[i];
        // if (set->materialList.materials[i].kD_index != -1) {
        //     index_slot++;
        //     set->materialList.materials[i].kD_index = index_slot;
        //     textures[materials[i].kD_index].storageImage->updateDescriptor(*set, 1, index_slot);
        // }
    }
}

TextureContainer::~TextureContainer() {
    destroy();
}
