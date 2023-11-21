cmake_minimum_required(VERSION 3.26)
project(vulkan_engine)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    include_directories("C:\\VulkanSDK\\glfw\\glfw-3.3.8.bin.WIN64\\include")
    include_directories("C:\\VulkanSDK\\1.3.268.0\\Include")
    include_directories("C:\\VulkanSDK\\glm")

    link_directories("C:\\VulkanSDK\\glfw\\glfw-3.3.8.bin.WIN64\\lib-vc2022")
    link_directories("C:\\VulkanSDK\\1.3.268.0\\Lib")
endif (WIN32)
find_package(Vulkan REQUIRED)
if (APPLE)
    include_directories("/Users/sherif/VulkanSDK/1.3.268.1/macOS/include")
    link_directories("/Users/sherif/VulkanSDK/1.3.268.1/macOS/lib")
    include_directories("/opt/homebrew/include")
    link_directories("/opt/homebrew/lib")
endif(APPLE)


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
        src/core/storage/DescriptorSampler.cpp
        src/core/storage/DescriptorSampler.h
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
)
if (APPLE)
    target_link_libraries(vulkan_engine "-framework CoreFoundation")
    target_link_libraries(vulkan_engine "-framework CoreGraphics")
    target_link_libraries(vulkan_engine "-framework IOKit")
    target_link_libraries(vulkan_engine "-framework Cocoa")
    target_link_libraries(vulkan_engine vulkan glfw shaderc_shared)
    target_include_directories(vulkan_engine PUBLIC ${VULKAN_INCLUDE_DIRS})
endif (APPLE)
