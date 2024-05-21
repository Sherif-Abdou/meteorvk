//
// Created by Sherif Abdou on 1/28/24.
//

#ifndef VULKAN_ENGINE_TEXTUREDESCRIPTORSET_H
#define VULKAN_ENGINE_TEXTUREDESCRIPTORSET_H


#include "core/storage/DescriptorSet.h"
#include "core_v2/NewDescriptorManager.h"
#include "engine/material/RenderMaterial.h"
#include "core/storage/StorageBuffer.h"

class TextureDescriptorSet {
public:
    static constexpr unsigned long MAX_MATERIALS = 64;
    static constexpr const char* MATERIAL_BUFFER_NAME = "material_buffer";
    struct MaterialList {
        RenderMaterial materials[MAX_MATERIALS];
    };
    MaterialList materialList;

    NewDescriptorManager* descriptorManager;

    void uploadMaterialList();
    explicit TextureDescriptorSet(VulkanContext* context);

    ~TextureDescriptorSet();

    const uint32_t max_images = 64;
    const uint32_t max_samplers = 8;

    static void attachLayoutToDescriptorManager(NewDescriptorManager* manager);
protected:
    StorageBuffer<MaterialList> material_storage_buffer;
};


#endif //VULKAN_ENGINE_TEXTUREDESCRIPTORSET_H
