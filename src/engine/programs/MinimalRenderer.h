//
// Created by Sherif Abdou on 5/7/24.
//

#ifndef VULKAN_ENGINE_MINIMALRENDERER_H
#define VULKAN_ENGINE_MINIMALRENDERER_H


#include "../../core/VulkanContext.h"
#include "../storage/ModelBuffer.h"
#include "../../core/storage/VertexBuffer.h"

class MinimalRenderer {
private:
    VulkanContext* context;
    ModelBuffer* buffer;

public:
    explicit MinimalRenderer(VulkanContext* context) : context(context) {}

    void run();

    VertexBuffer createVertexBuffer(VulkanContext *context, const char *path);
};


#endif //VULKAN_ENGINE_MINIMALRENDERER_H
