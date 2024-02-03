//
// Created by Sherif Abdou on 1/28/24.
//

#include "TextureDescriptorSet.h"

void TextureDescriptorSet::createDescriptorPool() {
//    DescriptorSet::createDescriptorPool();

    vk::DescriptorPoolSize samplerSize {};
    samplerSize.setType(vk::DescriptorType::eSampler);
    samplerSize.setDescriptorCount(max_samplers*frames_in_flight);

    vk::DescriptorPoolSize sampledImageSize {};
    sampledImageSize.setType(vk::DescriptorType::eSampledImage);
    sampledImageSize.setDescriptorCount(max_images*frames_in_flight);

    vk::DescriptorPoolSize storageSize {};
    storageSize.setType(vk::DescriptorType::eStorageBuffer);
    storageSize.setDescriptorCount(frames_in_flight);


    vk::DescriptorPoolCreateInfo createInfo {};
    auto sizes = std::vector<vk::DescriptorPoolSize> {samplerSize, sampledImageSize, storageSize};
    createInfo.setPoolSizes(sizes);
    createInfo.setMaxSets(frames_in_flight);
    createInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);

    descriptorPool = context->device.createDescriptorPool(createInfo).release();
}

void TextureDescriptorSet::createDescriptorSet() {
    vk::DescriptorSetAllocateInfo allocateInfo {};

    auto list = std::vector<vk::DescriptorSetLayout> {descriptorSetLayout};
//    list.push_back(descriptorSetLayout);
    allocateInfo.setDescriptorPool(descriptorPool);
    allocateInfo.setSetLayouts(list);

//    vk::DescriptorSetVariableDescriptorCountAllocateInfo variableAllocateInfo {};
//    std::vector<uint32_t> descriptor_counts {max_images};
//    variableAllocateInfo.setDescriptorCounts(descriptor_counts);
//
//    allocateInfo.setPNext(&variableAllocateInfo);

    descriptorSet = (*context->device).allocateDescriptorSets(allocateInfo);
}

void TextureDescriptorSet::createDescriptorLayout() {
    vk::DescriptorSetLayoutCreateInfo createInfo {};
    vk::DescriptorSetLayoutBindingFlagsCreateInfo flagsCreateInfo {};
    auto special_flags = vk::DescriptorBindingFlagBits::eUpdateAfterBind | vk::DescriptorBindingFlagBits::ePartiallyBound;
    std::vector<vk::DescriptorBindingFlags> flags {special_flags, special_flags, special_flags};
    flagsCreateInfo.setBindingFlags(flags);
    std::vector<vk::DescriptorSetLayoutBinding> bindings {};

    bindings.push_back(vk::DescriptorSetLayoutBinding()
        .setDescriptorCount(max_samplers)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
        .setDescriptorType(vk::DescriptorType::eSampler)
        .setBinding(0)
    );

    bindings.push_back(vk::DescriptorSetLayoutBinding()
        .setDescriptorCount(max_images)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
        .setDescriptorType(vk::DescriptorType::eSampledImage)
        .setBinding(1)
    );

    bindings.push_back(vk::DescriptorSetLayoutBinding()
        .setDescriptorCount(1)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment)
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setBinding(2));

    for (auto& binding: bindings) {
        if (binding.descriptorType == vk::DescriptorType::eUniformBufferDynamic || binding.descriptorType == vk::DescriptorType::eStorageBufferDynamic) {
            dynamic_offsets.push_back(0);
        }
    }

    createInfo.setFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool);
    createInfo.setPNext(&flagsCreateInfo);
    createInfo.setBindings(bindings);

    this->descriptorSetLayout = context->device.createDescriptorSetLayout(createInfo).release();
}

TextureDescriptorSet::TextureDescriptorSet(VulkanContext *context) : DescriptorSet(context, {}),
                                                                     material_storage_buffer(context) {
    frames_in_flight = 1;
    material_storage_buffer.allocateBuffer();
}

void TextureDescriptorSet::uploadMaterialList() {
    material_storage_buffer.updateBuffer(materialList);
    material_storage_buffer.writeToDescriptor(*this, 2);
}

TextureDescriptorSet::~TextureDescriptorSet() {
    material_storage_buffer.destroy();

}
