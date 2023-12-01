//
// Created by Sherif Abdou on 9/15/23.
//

#ifndef VULKAN_ENGINE_RENDERABLE_H
#define VULKAN_ENGINE_RENDERABLE_H

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "../storage/VertexBuffer.h"

class Renderable {
public:
    virtual ~Renderable() = default;

    struct RenderArguments {
        vk::raii::CommandBuffer& commandBuffer;
        uint32_t imageIndex;
        std::vector<VertexBuffer*> vertexBuffers;
    };

    virtual void renderPipeline(Renderable::RenderArguments renderArguments) = 0;

    virtual void prepareRender(Renderable::RenderArguments renderArguments) {};
};


#endif //VULKAN_ENGINE_RENDERABLE_H
