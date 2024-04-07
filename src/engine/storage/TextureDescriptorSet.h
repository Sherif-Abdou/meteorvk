//
// Created by Sherif Abdou on 1/28/24.
//

#ifndef VULKAN_ENGINE_TEXTUREDESCRIPTORSET_H
#define VULKAN_ENGINE_TEXTUREDESCRIPTORSET_H


#include "../../core/storage/DescriptorSet.h"
#include "../material/RenderMaterial.h"
#include "../../core/storage/StorageBuffer.h"

class TextureDescriptorSet: public DescriptorSet {
protected:
    void createDescriptorPool() override;

    void createDescriptorSet() override;

    void createDescriptorLayout() override;


public:
    static constexpr unsigned long MAX_MATERIALS = 256;
    struct MaterialList {
        RenderMaterial materials[MAX_MATERIALS];
    };
    MaterialList materialList;

    void uploadMaterialList();
    explicit TextureDescriptorSet(VulkanContext* context);

    ~TextureDescriptorSet() override;

    const uint32_t max_images = 100;
    const uint32_t max_samplers = 8;
protected:
    StorageBuffer<MaterialList> material_storage_buffer;
};


#endif //VULKAN_ENGINE_TEXTUREDESCRIPTORSET_H
