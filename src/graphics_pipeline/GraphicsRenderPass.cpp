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
    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependency.srcAccessMask = vk::AccessFlags();
    subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    return {
        subpassDescription,
        subpassDependency
    };
}

void GraphicsRenderPass::createAttachments() {
    colorDescription.setFormat(context.swapChainImageFormat);
    colorDescription.setInitialLayout(vk::ImageLayout::eUndefined);
    colorDescription.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    colorDescription.setFormat(context.swapChainImageFormat);
    colorDescription.setLoadOp(vk::AttachmentLoadOp::eClear);
    colorDescription.setStoreOp(vk::AttachmentStoreOp::eStore);
    colorDescription.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colorDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

    depthDescription.setInitialLayout(vk::ImageLayout::eUndefined);
    depthDescription.setFinalLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);
    depthDescription.setFormat(vk::Format::eD32Sfloat);
    depthDescription.setLoadOp(vk::AttachmentLoadOp::eClear);
    depthDescription.setStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthDescription.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    depthDescription.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
}

vk::raii::RenderPass & GraphicsRenderPass::getRenderPass() {
    return renderPass;
}

GraphicsRenderPass::GraphicsRenderPass(VulkanContext &context) : context(context) {}

void GraphicsRenderPass::init() {
    createRenderPass();
}
