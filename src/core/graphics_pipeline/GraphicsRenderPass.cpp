//
// Created by Sherif Abdou on 8/4/23.
//

#include "GraphicsRenderPass.h"

void GraphicsRenderPass::createRenderPass() {
    createAttachments();

    std::vector<vk::AttachmentDescription> attachments {};

    if (useColor) {
        attachments.push_back(colorDescription);
    }
    if (useDepth) {
        attachments.push_back(depthDescription);
    }
    if (multisampling) {
        attachments.push_back(colorDescriptionResolve);
    }

    auto subpasses = {subpass.description};
    auto dependencies = {subpass.dependency};
    vk::RenderPassCreateInfo createInfo {};
    createInfo.setSubpasses(subpasses);
    createInfo.setDependencies(dependencies);
    createInfo.setAttachments(attachments);

    renderPass = context->device.createRenderPass(createInfo);
}

GraphicsRenderPass::Subpass GraphicsRenderPass::createSubpass() {
    vk::SubpassDescription subpassDescription {};
    int i = 0;

    if (useColor) {
        //TODO: Potential Memory Leak
        auto* colorReference = new vk::AttachmentReference {};
        colorReference->setAttachment(i);
        colorReference->setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        subpassDescription.setColorAttachments(*colorReference);

        i++;
    }

    if (useDepth) {
        //TODO: Potential Memory Leak
        auto depthReference = new vk::AttachmentReference {};
        depthReference->setAttachment(i);
        depthReference->setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        subpassDescription.setPDepthStencilAttachment(depthReference);

        i++;
    }

    if (multisampling) {
        auto* resolveReference = new vk::AttachmentReference {};
        resolveReference->setAttachment(i);
        resolveReference->setLayout(vk::ImageLayout::eColorAttachmentOptimal);

        subpassDescription.setResolveAttachments(*resolveReference);
        i++;
    }

    subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    vk::SubpassDependency subpassDependency {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependency.dstAccessMask =  vk::AccessFlagBits::eColorAttachmentWrite;

    return {
        subpassDescription,
        subpassDependency
    };
}

void GraphicsRenderPass::createAttachments() {
    if (useColor && !custom_color) {
        colorDescription.setFormat(context->swapChainImageFormat)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
            .setFormat(context->swapChainImageFormat)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        if (multisampling) {
            colorDescription
                .setSamples(context->sampleCountFlagBits)
                .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
        }
    }

    if (useDepth) {
        depthDescription
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal)
            .setFormat(vk::Format::eD32Sfloat)
            .setSamples(colorDescription.samples)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(storeDepth ? vk::AttachmentStoreOp::eStore : vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    }

    if (multisampling) {
        colorDescriptionResolve.setFormat(context->swapChainImageFormat)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
                .setFormat(context->swapChainImageFormat)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    }
}

vk::raii::RenderPass & GraphicsRenderPass::getRenderPass() {
    return renderPass;
}

GraphicsRenderPass::GraphicsRenderPass(VulkanContext *context) : context(context) {
    if (colorFormat == vk::Format::eUndefined) {
        colorFormat = context->swapChainImageFormat;
    }
}

void GraphicsRenderPass::init() {
    subpass = createSubpass();
    createRenderPass();
}

void GraphicsRenderPass::init(vk::SubpassDependency dependency) {
    subpass = createSubpass();
    subpass.dependency = dependency;
    createRenderPass();
}

void GraphicsRenderPass::useCustomColor(vk::Format format, vk::ImageLayout finalLayout) {
    custom_color = true;

    colorDescription
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(finalLayout)
        .setFormat(format)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
}

GraphicsRenderPass* GraphicsRenderPass::shouldStoreDepth(bool option) {
  storeDepth = option;
  return this;
}

GraphicsRenderPass* GraphicsRenderPass::shouldUseDepth(bool option) {
  useDepth = option;
  return this;
}

GraphicsRenderPass* GraphicsRenderPass::useMultisampling(bool option) {
  multisampling = option;
  return this;
}
