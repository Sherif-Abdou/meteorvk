//
// Created by Sherif Abdou on 12/14/23.
//

#include "CullingComputePipeline.h"
#include "../../core/compute_pipeline/ComputePipelineBuilder.h"
#include "../../core/interfaces/IndirectCallStruct.h"
#include "../../core/compute_pipeline/ComputeCommandBuffer.h"
#include "../../core/storage/StorageBuffer.h"
#include "../programs/BackpackRenderer.h"
#include <glm/ext.hpp>

void CullingComputePipeline::generateIndirects() {
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

    struct UBO {
        glm::mat4 proj = glm::perspective(glm::radians(90.f), 1920.0f/1080.0f, 0.1f, 40.0f);
        glm::mat4 view = glm::translate(glm::rotate(glm::identity<glm::mat4>(), glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,-1,0));
        glm::mat4 model = glm::scale(glm::identity<glm::mat4>(), glm::vec3(100.1));
        uint32_t count = 0;
    };


    auto compute_pipeline_builder = ComputePipelineBuilder(context);
    auto shaders = ComputeShaders(*context, "shaders/cull.comp");

    output_buffer = std::make_unique<StorageBuffer<IndirectCallStruct>>(context);
    output_buffer->count = vertex_buffers.size();
    output_buffer->allocateBuffer();
    auto ubo_buffer = UniformBuffer<UBO>(context);
    ubo_buffer.allocateBuffer();
    auto ubo = UBO();

    auto descriptor = DescriptorSet(context, {uniform_binding, sb1_binding, sb2_binding, dynamic_ubo_binding});
    descriptor.buildDescriptor();

    compute_pipeline_builder.setShader(shaders);
    compute_pipeline_builder.setDescriptor(descriptor.getDescriptorSetLayout());
    auto pipeline = compute_pipeline_builder.build();

    for (uint32_t i = 0; i < vertex_buffers.size(); i++) {
        auto vertices = vertex_buffers[i];
        ubo.count = vertices->getVertexCount();
        ubo_buffer.updateBuffer(ubo);
        auto buffer_write_info = vk::DescriptorBufferInfo(*vertices->getBuffer(), 0, vertices->getSize());
        vk::WriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet
                .setDescriptorType(vk::DescriptorType::eStorageBuffer)
                .setDstArrayElement(0)
                .setDstBinding(1)
                .setDstSet(descriptor.getDescriptorSet())
                .setDescriptorCount(1)
                .setBufferInfo(buffer_write_info);


        auto initial = vertices->createBasicIndirectCall();
        initial.vertexCount = 0;
        output_buffer->updateBuffer(initial, i);

        pipeline.workgroups = std::floor(vertices->getVertexCount() / 256);

        auto compute_command = ComputeCommandBuffer(context);
        compute_command.init();
        compute_command.begin();
        models->writeBuffer(descriptor, 3);
        models->attachOffsetToDescriptor(descriptor, i);
        context->device.updateDescriptorSets(writeDescriptorSet, {});
        output_buffer->writePartialToDescriptor(descriptor, sizeof (IndirectCallStruct), sizeof (IndirectCallStruct) * i, 2);
        ubo_buffer.writeToDescriptor(descriptor, 0);
        compute_command.bindAndDispatch(pipeline, &descriptor);
        compute_command.end();
        compute_command.submit();
        context->device.waitIdle();
    }

    IndirectCallStruct* raw = (IndirectCallStruct*)output_buffer->mapMemory();

    std::cout << raw[0].vertexCount << "\n";
    std::cout << raw[1].vertexCount << "\n";

    output_buffer->unMapMemory();


    context->device.waitIdle();
    output_buffer->destroy();
    ubo_buffer.destroy();
}

CullingComputePipeline::CullingComputePipeline(VulkanContext *context) : context(context) {}
