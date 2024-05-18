#ifndef VULKAN_ENGINE_RENDERABLE_CHAIN
#define VULKAN_ENGINE_RENDERABLE_CHAIN

#include "../../core/graphics_pipeline/GraphicsCommandBuffer.h"
#include "../../core/interfaces/Renderable.h"
#include "../../core/shared_pipeline/PipelineBarrier.h"
#include "../../core/VulkanContext.h"
#include "../NewDescriptorManager.h"

// Helper class to merge a series of renderables to be used in a commandbuffer
class RenderableChain {
public:
    struct RenderableContext {
        Renderable* renderable;
        // Needed in order to bind the appropriate descriptors for each renderable
        vk::raii::PipelineLayout* pipeline_layout;
        std::vector<PipelineBarrier> barriers;
    };
private:
    VulkanContext* context;
    NewDescriptorManager* descriptorManager;

    std::vector<RenderableContext> renderable_contexts;
public:
    explicit RenderableChain(VulkanContext* context, NewDescriptorManager* descriptorManager);

    void applyToCommandBuffer(GraphicsCommandBuffer* command_buffer);
};

#endif
