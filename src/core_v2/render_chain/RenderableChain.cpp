#include "RenderableChain.h"


RenderableChain::RenderableChain(VulkanContext* context, NewDescriptorManager* descriptorManager): context(context), descriptorManager(descriptorManager) {

}


void RenderableChain::applyToCommandBuffer(GraphicsCommandBuffer* command_buffer) {
    int i = 0;
    for (auto& context : this->renderable_contexts) {
        command_buffer->pipelines.push_back(context.renderable);
        command_buffer->dependencies[i] = context.barriers;
        command_buffer->bindings.push_back({
                descriptorManager->getDescriptorFor(NewDescriptorManager::BindingUpdateRate::Frame),
                context.pipeline_layout,
                0
                });
        i++;
    }

}

void RenderableChain::addRenderable(const RenderableContext& cont) {
    this->renderable_contexts.push_back(cont);
}
