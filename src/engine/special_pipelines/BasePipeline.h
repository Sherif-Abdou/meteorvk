#ifndef VULKAN_ENGINE_BASEPIPELINE_H
#define VULKAN_ENGINE_BASEPIPELINE_H

#include "vulkan/vulkan_raii.hpp"

#include "core/interfaces/Renderable.h"
#include "core/graphics_pipeline/GraphicsPipeline.h"
#include "core_v2/NewDescriptorManager.h"

// Generic Specialized Pipeline
class BasePipeline: public Renderable {
    public:
        std::string pipeline_name = "";

        NewDescriptorManager* descriptors;

        BasePipeline() = default;

        // Helper constructor to support composition of special pipelines
        BasePipeline(BasePipeline* super);

        virtual GraphicsPipeline& getGraphicsPipeline() = 0;

        virtual ~BasePipeline() = default;

        virtual void destroy() {};
    protected:
        DescriptorSet* local_descriptor = nullptr;

        void tryLoadLocalDescriptor();
        
        void tryBindLocalDescriptor(vk::raii::CommandBuffer *commandBuffer);
};

#endif
