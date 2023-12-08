//
// Created by Sherif Abdou on 9/28/23.
//

#ifndef VULKAN_ENGINE_MODELBUFFERGRAPHICSPIPELINE_H
#define VULKAN_ENGINE_MODELBUFFERGRAPHICSPIPELINE_H


#include "../../core/interfaces/Renderable.h"
#include "../../core/graphics_pipeline/GraphicsPipeline.h"
#include "../storage/ModelBuffer.h"
#include "../../core/graphics_pipeline/GraphicsPipelineBuilder.h"

/// Graphics pipeline that adjusts the model buffer per vertexbuffer
/// Modelbuffer indices must be in the same order as the vertexbuffers
class ModelBufferGraphicsPipeline: public Renderable {
private:
    GraphicsPipeline graphicsPipeline;
public:
    explicit ModelBufferGraphicsPipeline(GraphicsPipeline &&graphicsPipeline,
                                         unsigned int size);

    explicit ModelBufferGraphicsPipeline(GraphicsPipeline &&graphicsPipeline,
                                         ModelBuffer* modelBuffer);

    static ModelBufferGraphicsPipeline
    createPipelineFromBuilder(GraphicsPipelineBuilder &&builder, ModelBuffer *modelBuffer, DescriptorSet* descriptor);

    GraphicsPipeline & getGraphicsPipeline();

    DescriptorSet* descriptorSet;
    ModelBuffer* modelBuffer = nullptr;

    void renderPipeline(Renderable::RenderArguments renderArguments) override;

    void prepareRender(Renderable::RenderArguments renderArguments) override;

    void destroy();
};


#endif //VULKAN_ENGINE_MODELBUFFERGRAPHICSPIPELINE_H
