cmake_minimum_required(VERSION 3.26)
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.1.2" CACHE STRING "Minimum OS X deployment version")
project(vulkan_engine)

set(CMAKE_CXX_STANDARD 20)

find_library(VULKAN_LIBRARY REQUIRED NAMES vulkan vulkan-1)
find_path(VULKAN_HEADERS REQUIRED NAMES vulkan)
find_path(VMA_HEADERS REQUIRED NAMES vma)
find_path(GLM_HEADERS REQUIRED NAMES glm)
find_library(SHADER_C_LIBRARY REQUIRED NAMES shaderc_shared)
find_path(SHADER_C_HEADERS REQUIRED NAMES shaderc)
find_library(GLFW_LIBRARY REQUIRED NAMES glfw glfw3)
find_path(GLFW_HEADERS REQUIRED NAMES "GLFW")
variable_watch(VULKAN_HEADERS)

add_executable(vulkan_engine main.cpp
        src/core/VulkanContext.h
        src/core/VulkanContext.cpp
        src/core/storage/VulkanAllocator.h
        src/core/storage/VulkanAllocator.cpp
        src/core/storage/Vertex.h
        src/core/storage/Vertex.cpp
        src/core/storage/VertexBuffer.h
        src/core/storage/VertexBuffer.cpp
        src/core/storage/DescriptorSet.cpp
        src/core/storage/DescriptorSet.h
        src/core/graphics_pipeline/GraphicsPipeline.cpp
        src/core/graphics_pipeline/GraphicsPipeline.h
        src/core/graphics_pipeline/GraphicsRenderPass.cpp
        src/core/graphics_pipeline/GraphicsRenderPass.h
        src/core/graphics_pipeline/GraphicsShaders.cpp
        src/core/graphics_pipeline/GraphicsShaders.h
        src/core/graphics_pipeline/GraphicsCommandBuffer.cpp
        src/core/graphics_pipeline/GraphicsCommandBuffer.h
        src/core/storage/OBJFile.h
        src/core/storage/OBJFile.cpp
        src/core/storage/UniformBuffer.cpp
        src/core/storage/UniformBuffer.h
        src/core/graphics_pipeline/GraphicsPipelineBuilder.cpp
        src/core/graphics_pipeline/GraphicsPipelineBuilder.h
        src/core/storage/CombinedDescriptorSampler.cpp
        src/core/storage/CombinedDescriptorSampler.h
        src/engine/special_pipelines/ShadowGraphicsPipeline.cpp
        src/engine/special_pipelines/ShadowGraphicsPipeline.h
        src/core/interfaces/Renderable.cpp
        src/core/interfaces/Renderable.h
        src/engine/special_pipelines/ForwardRenderedGraphicsPipeline.cpp
        src/engine/special_pipelines/ForwardRenderedGraphicsPipeline.h
        src/core/storage/DynamicUniformBuffer.cpp
        src/core/storage/DynamicUniformBuffer.h
        src/engine/storage/ModelBuffer.cpp
        src/engine/storage/ModelBuffer.h
        src/engine/special_pipelines/ModelBufferGraphicsPipeline.cpp
        src/engine/special_pipelines/ModelBufferGraphicsPipeline.h
        src/core/compute_pipeline/ComputePipeline.cpp
        src/core/compute_pipeline/ComputePipeline.h
        src/core/compute_pipeline/ComputePipelineBuilder.cpp
        src/core/compute_pipeline/ComputePipelineBuilder.h
        src/core/compute_pipeline/ComputeShaders.cpp
        src/core/compute_pipeline/ComputeShaders.h
        src/core/storage/StorageBuffer.cpp
        src/core/storage/StorageBuffer.h
        src/core/compute_pipeline/ComputeCommandBuffer.cpp
        src/core/compute_pipeline/ComputeCommandBuffer.h
        src/core/shared_pipeline/PipelineBarrier.cpp
        src/core/shared_pipeline/PipelineBarrier.h
        src/core/shared_pipeline/PipelineBarrierBuilder.cpp
        src/core/shared_pipeline/PipelineBarrierBuilder.h
        src/engine/material/Material.cpp
        src/engine/material/Material.h
        src/engine/storage/ImageTextureLoader.cpp
        src/engine/storage/ImageTextureLoader.h
        stbi/stbi_image.h
        src/core/shared_pipeline/TransferQueue.cpp
        src/core/shared_pipeline/TransferQueue.h
        src/engine/special_pipelines/SSAOGraphicsPipeline.cpp
        src/engine/special_pipelines/SSAOGraphicsPipeline.h
        src/core/storage/DescriptorSampler.cpp
        src/core/storage/DescriptorSampler.h
        src/core/storage/StorageImage.cpp
        src/core/storage/StorageImage.h
        src/engine/special_pipelines/DepthOnlyPipeline.cpp
        src/engine/special_pipelines/DepthOnlyPipeline.h
        src/engine/programs/BackpackRenderer.cpp
        src/engine/programs/BackpackRenderer.h
        src/core/interfaces/IndirectCallStruct.cpp
        src/core/interfaces/IndirectCallStruct.h
        src/engine/special_pipelines/CullingComputePipeline.cpp
        src/engine/special_pipelines/CullingComputePipeline.h
        src/engine/special_pipelines/PostGraphicsPipeline.cpp
        src/engine/special_pipelines/PostGraphicsPipeline.h
        src/engine/special_pipelines/GeometryGraphicsPipeline.cpp
        src/engine/special_pipelines/GeometryGraphicsPipeline.h
        src/engine/storage/TextureDescriptorSet.cpp
        src/engine/storage/TextureDescriptorSet.h
        src/engine/storage/TextureContainer.cpp
        src/engine/storage/TextureContainer.h
)
# Apple MoltenVK Requirements
if (APPLE)
    target_link_libraries(vulkan_engine "-framework CoreFoundation")
    target_link_libraries(vulkan_engine "-framework CoreGraphics")
    target_link_libraries(vulkan_engine "-framework IOKit")
    target_link_libraries(vulkan_engine "-framework Cocoa")
endif (APPLE)
#    target_link_libraries(vulkan_engine vulkan glfw shaderc_shared)
target_link_libraries(vulkan_engine ${VULKAN_LIBRARY} ${SHADER_C_LIBRARY} ${GLFW_LIBRARY})
target_include_directories(vulkan_engine PUBLIC ${VULKAN_HEADERS} ${SHADER_C_HEADERS} ${GLFW_HEADERS} ${VMA_HEADERS} ${GLM_HEADERS})
