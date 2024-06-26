#ifndef VULKAN_ENGINE_BASEPIPELINE_H
#define VULKAN_ENGINE_BASEPIPELINE_H

#include "../../core/interfaces/Renderable.h"
#include "../../core/graphics_pipeline/GraphicsPipeline.h"
#include "../storage/DescriptorManager.h"

// Generic Specialized Pipeline
class BasePipeline: public Renderable {
  public:
    DescriptorManager* descriptors;

    BasePipeline() = default;

    // Helper constructor to support composition of special pipelines
    BasePipeline(BasePipeline* super);

    virtual GraphicsPipeline& getGraphicsPipeline() = 0;

    virtual ~BasePipeline() = default;

    virtual void destroy() {};
};

#endif
