//
// Created by Sherif Abdou on 12/14/23.
//

#ifndef VULKAN_ENGINE_CULLINGCOMPUTEPIPELINE_H
#define VULKAN_ENGINE_CULLINGCOMPUTEPIPELINE_H


#include "../storage/ModelBuffer.h"
#include "../../core/compute_pipeline/ComputePipeline.h"
#include "../../core/storage/VertexBuffer.h"
#include "../../core/storage/StorageBuffer.h"

class CullingComputePipeline {
public:
    ModelBuffer* models;
    std::vector<VertexBuffer*> vertex_buffers;
    VulkanContext* context;

    explicit CullingComputePipeline(VulkanContext *context);

    std::unique_ptr<StorageBuffer<IndirectCallStruct>> output_buffer;

    void generateIndirects();
};


#endif //VULKAN_ENGINE_CULLINGCOMPUTEPIPELINE_H
