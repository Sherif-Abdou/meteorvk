//
// Created by Sherif Abdou on 8/4/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSRENDERPASS_H
#define VULKAN_ENGINE_GRAPHICSRENDERPASS_H

#include "../VulkanContext.h"

class GraphicsRenderPass {
public:
    VulkanContext& context;
    struct Subpass {
        vk::SubpassDescription description;
        vk::SubpassDependency dependency;
    };

    explicit GraphicsRenderPass(VulkanContext &context);

    vk::raii::RenderPass & getRenderPass();

    void init();
private:
    vk::raii::RenderPass renderPass = nullptr;

private:
    vk::AttachmentDescription colorDescription {};
    vk::AttachmentDescription depthDescription {};

    void createRenderPass();

    Subpass createSubpass();

    void createAttachments();
};


#endif //VULKAN_ENGINE_GRAPHICSRENDERPASS_H