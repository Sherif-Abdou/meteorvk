//
// Created by Sherif Abdou on 12/14/23.
//

#include "CullingComputePipeline.h"
#include "../../core/compute_pipeline/ComputePipelineBuilder.h"
#include "../../core/interfaces/IndirectCallStruct.h"
#include "../../core/compute_pipeline/ComputeCommandBuffer.h"
#include "../../core/storage/StorageBuffer.h"
#include "../programs/BackpackRenderer.h"

void CullingComputePipeline::generateIndirects() {
    models->resetIndex();
    for (uint32_t i = 0; i < vertex_buffers.size(); i++) {
        auto vertices = vertex_buffers[i];
        ubo.count = vertices->getVertexCount();
        ubo_buffer->updateBuffer(ubo);
        auto buffer_write_info = vk::DescriptorBufferInfo(*vertices->getBuffer(), 0, vertices->getSize());
        vk::WriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet
                .setDescriptorType(vk::DescriptorType::eStorageBuffer)
                .setDstArrayElement(0)
                .setDstBinding(1)
                .setDstSet(descriptor->getDescriptorSet())
                .setDescriptorCount(1)
                .setBufferInfo(buffer_write_info);


        auto initial = vertices->createBasicIndirectCall();
        initial.indexCount = 0;
        output_buffer->updateBuffer(initial, i);

        pipeline->workgroups = std::floor(vertices->getVertexCount() / 256.0f);
        if (pipeline->workgroups == 0) {
            pipeline->workgroups = 1;
        }

        auto compute_command = ComputeCommandBuffer(context);
        compute_command.init();
        compute_command.begin();
        models->attachOffsetToDescriptor(*descriptor, 0);
        models->writeBuffer(*descriptor, 3);
        models->attachOffsetToDescriptor(*descriptor, 0);
        models->nextIndex();
        context->device.updateDescriptorSets(writeDescriptorSet, {});
        output_buffer->writePartialToDescriptor(*descriptor, sizeof (IndirectCallStruct), sizeof (IndirectCallStruct) * i, 2);
        ubo_buffer->writeToDescriptor(*descriptor, 0);
        compute_command.bindAndDispatch(*pipeline, descriptor.get());
        compute_command.end();
        compute_command.submit();
        context->device.waitIdle();
    }



}

CullingComputePipeline::CullingComputePipeline(VulkanContext *context) : context(context) {}

void CullingComputePipeline::init() {
    auto uniform_binding = vk::DescriptorSetLayoutBinding()
            .setBinding(0)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setStageFlags(vk::ShaderStageFlagBits::eCompute);
    auto sb1_binding = vk::DescriptorSetLayoutBinding()
            .setBinding(1)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eStorageBuffer)
            .setStageFlags(vk::ShaderStageFlagBits::eCompute);

    auto sb2_binding = vk::DescriptorSetLayoutBinding()
            .setBinding(2)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eStorageBuffer)
            .setStageFlags(vk::ShaderStageFlagBits::eCompute);

    auto dynamic_ubo_binding = vk::DescriptorSetLayoutBinding()
            .setBinding(3)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
            .setStageFlags(vk::ShaderStageFlagBits::eCompute);


    auto compute_pipeline_builder = ComputePipelineBuilder(context);
    auto shaders = ComputeShaders(*context, "shaders/cull.comp");

    output_buffer = std::make_unique<StorageBuffer<IndirectCallStruct>>(context, true);
    output_buffer->count = vertex_buffers.size();
    output_buffer->allocateBuffer();
    ubo_buffer = std::make_unique<UniformBuffer<UBO>>(context);
    ubo_buffer->allocateBuffer();

    descriptor = std::make_unique<DescriptorSet>(context, std::vector<vk::DescriptorSetLayoutBinding> {uniform_binding, sb1_binding, sb2_binding, dynamic_ubo_binding});
    descriptor->buildDescriptor();

    compute_pipeline_builder.setShader(shaders);
    compute_pipeline_builder.setDescriptor(descriptor->getDescriptorSetLayout());
    pipeline = compute_pipeline_builder.build();
}

void CullingComputePipeline::destroy() {
    ubo_buffer->destroy();
    output_buffer->destroy();
}
