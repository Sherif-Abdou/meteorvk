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

    auto subpasses = {subpass.description};
    auto dependencies = {subpass.dependency};
    vk::RenderPassCreateInfo createInfo {};
    createInfo.setSubpasses(subpasses);
    createInfo.setDependencies(dependencies);
    createInfo.setAttachments(attachments);

    renderPass = context.device.createRenderPass(createInfo);
}

GraphicsRenderPass::Subpass GraphicsRenderPass::createSubpass() {
    vk::SubpassDescription subpassDescription {};

    if (useColor) {
        //TODO: Potential Memory Leak
        auto* colorReference = new vk::AttachmentReference {};
        colorReference->setAttachment(0);
        colorReference->setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        subpassDescription.setColorAttachments(*colorReference);
    }

    if (useDepth) {
        //TODO: Potential Memory Leak
        auto depthReference = new vk::AttachmentReference {};
        depthReference->setAttachment(1 ? useColor : 0);
        depthReference->setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        subpassDescription.setPDepthStencilAttachment(depthReference);
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
    if (useColor) {
        colorDescription.setFormat(context.swapChainImageFormat)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
            .setFormat(context.swapChainImageFormat)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    }

    if (useDepth) {
        depthDescription
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal)
            .setFormat(vk::Format::eD32Sfloat)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    }
}

vk::raii::RenderPass & GraphicsRenderPass::getRenderPass() {
    return renderPass;
}

GraphicsRenderPass::GraphicsRenderPass(VulkanContext &context) : context(context) {
    if (colorFormat == vk::Format::eUndefined) {
        colorFormat = context.swapChainImageFormat;
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
