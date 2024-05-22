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

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.45.0 COMPONENTS regex container)


add_executable(vulkan_engine main.cpp
        src/core/VulkanContext.cpp
        src/core/storage/VulkanAllocator.cpp
        src/core/storage/Vertex.cpp
        src/core/storage/VertexBuffer.cpp
        src/core/storage/DescriptorSet.cpp
        src/core/graphics_pipeline/GraphicsPipeline.cpp
        src/core/graphics_pipeline/GraphicsRenderPass.cpp
        src/core/graphics_pipeline/GraphicsShaders.cpp
        src/core/graphics_pipeline/GraphicsCommandBuffer.cpp
        src/core/storage/OBJFile.cpp
        src/core/storage/UniformBuffer.cpp
        src/core/graphics_pipeline/GraphicsPipelineBuilder.cpp
        src/core/storage/CombinedDescriptorSampler.cpp
        src/engine/special_pipelines/ShadowGraphicsPipeline.cpp
        src/core/interfaces/Renderable.cpp
        src/engine/special_pipelines/ForwardRenderedGraphicsPipeline.cpp
        src/core/storage/DynamicUniformBuffer.cpp
        src/engine/storage/ModelBuffer.cpp
        src/engine/special_pipelines/ModelBufferGraphicsPipeline.cpp
        src/core/compute_pipeline/ComputePipeline.cpp
        src/core/compute_pipeline/ComputePipelineBuilder.cpp
        src/core/compute_pipeline/ComputeShaders.cpp
        src/core/storage/StorageBuffer.cpp
        src/core/compute_pipeline/ComputeCommandBuffer.cpp
        src/core/shared_pipeline/PipelineBarrier.cpp
        src/core/shared_pipeline/PipelineBarrierBuilder.cpp
        src/engine/material/Material.cpp
        src/engine/storage/ImageTextureLoader.cpp
        src/core/shared_pipeline/TransferQueue.cpp
        src/engine/special_pipelines/SSAOGraphicsPipeline.cpp
        src/core/storage/DescriptorSampler.cpp
        src/core/storage/StorageImage.cpp
        src/engine/special_pipelines/DepthOnlyPipeline.cpp
        src/core/interfaces/IndirectCallStruct.cpp
        src/engine/special_pipelines/CullingComputePipeline.cpp
        src/engine/special_pipelines/PostGraphicsPipeline.cpp
        src/engine/special_pipelines/GeometryGraphicsPipeline.cpp
        src/engine/storage/TextureDescriptorSet.cpp
        src/engine/storage/TextureContainer.cpp
        src/engine/material/RenderMaterial.cpp
        src/engine/storage/MTLFile.cpp
        src/core/storage/NewOBJFile.cpp
        src/engine/storage/DescriptorManager.cpp
        src/core_v2/render_chain/RenderableChain.cpp
        src/core_v2/GraphicsPipelineBuilder2.cpp
        src/core_v2/NewDescriptorManager.cpp
        src/core_v2/storage/CustomDescriptorSet.cpp
        src/engine/programs/MinimalRenderer.cpp
        src/engine/programs/NewRenderer.cpp
        src/engine/storage/LightBuffer.cpp
        src/engine/storage/VertexBufferLoader.cpp
)
# Apple MoltenVK Requirements
if (APPLE)
    target_link_libraries(vulkan_engine "-framework CoreFoundation")
    target_link_libraries(vulkan_engine "-framework CoreGraphics")
    target_link_libraries(vulkan_engine "-framework IOKit")
    target_link_libraries(vulkan_engine "-framework Cocoa")
endif (APPLE)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(vulkan_engine ${Boost_LIBRARIES})
endif()
#    target_link_libraries(vulkan_engine vulkan glfw shaderc_shared)
target_link_libraries(vulkan_engine ${VULKAN_LIBRARY} ${SHADER_C_LIBRARY} ${GLFW_LIBRARY})
target_include_directories(vulkan_engine PUBLIC ${VULKAN_HEADERS} ${SHADER_C_HEADERS} ${GLFW_HEADERS} ${VMA_HEADERS} ${GLM_HEADERS} "./src/")
