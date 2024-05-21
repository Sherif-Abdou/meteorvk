//
// Created by Sherif Abdou on 8/4/23.
//

#ifndef VULKAN_ENGINE_GRAPHICSRENDERPASS_H
#define VULKAN_ENGINE_GRAPHICSRENDERPASS_H

#include "core/VulkanContext.h"

/// Wrappper over VkRenderPass
class GraphicsRenderPass {
public:
    VulkanContext* context;
    struct Subpass {
        vk::SubpassDescription description;
        vk::SubpassDependency dependency;
    };

    explicit GraphicsRenderPass(VulkanContext *context);

    vk::raii::RenderPass & getRenderPass();

    void init();
    void init(vk::SubpassDependency dependency);

    void useCustomColor(vk::Format format, vk::ImageLayout finalLayout);
    bool useColor = true;
    bool useDepth = true;
    bool storeDepth = false;
    bool multisampling = false;

    GraphicsRenderPass* shouldStoreDepth(bool option);
    GraphicsRenderPass* shouldUseDepth(bool option);
    GraphicsRenderPass* useMultisampling(bool option);
    vk::Format colorFormat = vk::Format::eUndefined;
private:
    vk::raii::RenderPass renderPass = nullptr;

private:
    bool custom_color = false;
    vk::AttachmentDescription colorDescription {};
    vk::AttachmentDescription depthDescription {};
    vk::AttachmentDescription colorDescriptionResolve {};

    Subpass subpass;

    void createRenderPass();

    Subpass createSubpass();

    void createAttachments();
};


#endif //VULKAN_ENGINE_GRAPHICSRENDERPASS_H
