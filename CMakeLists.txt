cmake_minimum_required(VERSION 3.26)
project(vulkan_engine)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    include_directories("C:\\VulkanSDK\\glfw\\glfw-3.3.8.bin.WIN64\\include")
    include_directories("C:\\VulkanSDK\\1.3.250.0\\Include")
    include_directories("C:\\VulkanSDK\\glm")

    link_directories("C:\\VulkanSDK\\glfw\\glfw-3.3.8.bin.WIN64\\lib-vc2022")
    link_directories("C:\\VulkanSDK\\1.3.250.0\\Lib")
endif (WIN32)
find_package(Vulkan REQUIRED)
if (APPLE)
    include_directories("/Users/sherif/VulkanSDK/1.3.250.1/macOS/include")
    link_directories("/Users/sherif/VulkanSDK/1.3.250.1/macOS/lib")
    include_directories("/opt/homebrew/include")
    link_directories("/opt/homebrew/lib")
endif(APPLE)


add_executable(vulkan_engine main.cpp
        src/VulkanContext.h
        src/VulkanContext.cpp
        src/storage/VulkanAllocator.h
        src/storage/VulkanAllocator.cpp
        src/storage/Vertex.h
        src/storage/Vertex.cpp
        src/storage/VertexBuffer.h
        src/storage/VertexBuffer.cpp
        src/storage/DescriptorSet.cpp
        src/storage/DescriptorSet.h
        src/graphics_pipeline/GraphicsPipeline.cpp
        src/graphics_pipeline/GraphicsPipeline.h
        src/graphics_pipeline/GraphicsRenderPass.cpp
        src/graphics_pipeline/GraphicsRenderPass.h
        src/graphics_pipeline/GraphicsShaders.cpp
        src/graphics_pipeline/GraphicsShaders.h
        src/graphics_pipeline/GraphicsCommandBuffer.cpp
        src/graphics_pipeline/GraphicsCommandBuffer.h
        src/storage/OBJFile.h
        src/storage/OBJFile.cpp
        src/storage/UniformBuffer.cpp
        src/storage/UniformBuffer.h
        src/graphics_pipeline/GraphicsPipelineBuilder.cpp
        src/graphics_pipeline/GraphicsPipelineBuilder.h
        src/storage/DescriptorSampler.cpp
        src/storage/DescriptorSampler.h
        src/graphics_pipeline/special_pipelines/ShadowGraphicsPipeline.cpp
        src/graphics_pipeline/special_pipelines/ShadowGraphicsPipeline.h
        src/interfaces/Renderable.cpp
        src/interfaces/Renderable.h
        src/graphics_pipeline/special_pipelines/ForwardRenderedGraphicsPipeline.cpp
        src/graphics_pipeline/special_pipelines/ForwardRenderedGraphicsPipeline.h
)
if (APPLE)
    target_link_libraries(vulkan_engine "-framework CoreFoundation")
    target_link_libraries(vulkan_engine "-framework CoreGraphics")
    target_link_libraries(vulkan_engine "-framework IOKit")
    target_link_libraries(vulkan_engine "-framework Cocoa")
    target_link_libraries(vulkan_engine vulkan glfw shaderc_shared)
    target_include_directories(vulkan_engine PUBLIC ${VULKAN_INCLUDE_DIRS})
endif (APPLE)
