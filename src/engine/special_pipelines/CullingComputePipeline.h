//
// Created by Sherif Abdou on 12/14/23.
//

#ifndef VULKAN_ENGINE_CULLINGCOMPUTEPIPELINE_H
#define VULKAN_ENGINE_CULLINGCOMPUTEPIPELINE_H


#include "../storage/ModelBuffer.h"
#include "../../core/compute_pipeline/ComputePipeline.h"
#include "../../core/storage/VertexBuffer.h"
#include "../../core/storage/StorageBuffer.h"
#include <glm/ext.hpp>

class CullingComputePipeline {
public:
    struct UBO {
        glm::mat4 proj = glm::perspective(glm::radians(90.f), 1920.0f/1080.0f, 0.1f, 40.0f);
        glm::mat4 view = glm::translate(glm::rotate(glm::identity<glm::mat4>(), glm::radians(180.0f), glm::vec3(0,1,0)), glm::vec3(0,-1,0));
        glm::mat4 model = glm::scale(glm::identity<glm::mat4>(), glm::vec3(100.1));
        uint32_t count = 0;
    };
private:
    std::unique_ptr<DescriptorSet> descriptor = nullptr;
    std::optional<ComputePipeline> pipeline;
    std::unique_ptr<UniformBuffer<UBO>> ubo_buffer;
public:
    UBO ubo = UBO();
    ModelBuffer* models;
    std::vector<VertexBuffer*> vertex_buffers;
    VulkanContext* context;

    explicit CullingComputePipeline(VulkanContext *context);

    std::unique_ptr<StorageBuffer<IndirectCallStruct>> output_buffer;

    void init();
    void generateIndirects();
    void destroy();
};


#endif //VULKAN_ENGINE_CULLINGCOMPUTEPIPELINE_H
