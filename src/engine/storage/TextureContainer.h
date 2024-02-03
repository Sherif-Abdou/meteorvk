//
// Created by Sherif Abdou on 1/30/24.
//

#ifndef VULKAN_ENGINE_TEXTURECONTAINER_H
#define VULKAN_ENGINE_TEXTURECONTAINER_H


#include "../../core/VulkanContext.h"
#include "../../core/storage/StorageImage.h"
#include "../../core/storage/DescriptorSampler.h"
#include "../material/RenderMaterial.h"
#include "TextureDescriptorSet.h"

class TextureContainer {
public:
    struct Texture {
        VulkanAllocator::VulkanImageAllocation allocation;
        vk::raii::ImageView imageView;
        StorageImage* storageImage;
    };

private:
    std::vector<Texture> textures {};
    std::vector<DescriptorSampler*> samplers {};
    std::vector<RenderMaterial> materials {};
public:
    VulkanContext* context;

    uint32_t addTexture(Texture texture);
    uint32_t addTexture(VulkanAllocator::VulkanImageAllocation allocation, vk::raii::ImageView imageView, StorageImage* storageImage);
    uint32_t addMaterial(RenderMaterial material);

    uint32_t addSampler(DescriptorSampler* sampler);

    uint32_t getTextureSize();
    uint32_t getSamplerSize();

    StorageImage* getStorageImage(uint32_t index);
    vk::raii::ImageView* getImageView(uint32_t index);
    VulkanAllocator::VulkanImageAllocation* getAllocation(uint32_t index);

    Texture& operator[](uint32_t index);

    void copyMaterialsTo(TextureDescriptorSet* set);

    void destroy();
};


#endif //VULKAN_ENGINE_TEXTURECONTAINER_H
