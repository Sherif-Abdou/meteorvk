//
// Created by Sherif Abdou on 8/4/23.
//

#include "GraphicsRenderPass.h"

void GraphicsRenderPass::createRenderPass() {
    createAttachments();
    auto subpass = createSubpass();

    auto attachments = {colorDescription, depthDescription};
    auto subpasses = {subpass.description};
    auto dependencies = {subpass.dependency};
    vk::RenderPassCreateInfo createInfo {};
    createInfo.setSubpasses(subpasses);
    createInfo.setDependencies(dependencies);
    createInfo.setAttachments(attachments);

    renderPass = context.device.createRenderPass(createInfo);
}

GraphicsRenderPass::Subpass GraphicsRenderPass::createSubpass() {
    auto* colorReference = new vk::AttachmentReference {};
    colorReference->setAttachment(0);
    colorReference->setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    auto depthReference = new vk::AttachmentReference {};
    depthReference->setAttachment(1);
    depthReference->setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpassDescription {};
    subpassDescription.setColorAttachments(*colorReference);
    subpassDescription.setPDepthStencilAttachment(depthReference);
    subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    vk::SubpassDependency subpassDependency {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    subpassDependency.srcAccessMask = vk::AccessFlags();
    subpassDependency.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    return {
        subpassDescription,
        subpassDependency
    };
}

void GraphicsRenderPass::createAttachments() {
    colorDescription.setFormat(context.swapChainImageFormat)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
        .setFormat(context.swapChainImageFormat)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

    depthDescription
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal)
        .setFormat(vk::Format::eD32Sfloat)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
}

vk::raii::RenderPass & GraphicsRenderPass::getRenderPass() {
    return renderPass;
}

GraphicsRenderPass::GraphicsRenderPass(VulkanContext &context) : context(context) {}

void GraphicsRenderPass::init() {
    createRenderPass();
}
