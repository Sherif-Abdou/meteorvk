#include <iostream>

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define VMA_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "src/VulkanContext.h"
#include "src/storage/VertexBuffer.h"
#include "src/graphics_pipeline/GraphicsRenderPass.h"
#include "src/graphics_pipeline/GraphicsPipeline.h"
#include "src/graphics_pipeline/GraphicsCommandBuffer.h"
#include "src/storage/UniformBuffer.h"
#include "src/graphics_pipeline/GraphicsPipelineBuilder.h"
#include "src/storage/DescriptorSampler.h"
#include "src/storage/OBJFile.h"
#include "src/graphics_pipeline/special_pipelines/ShadowGraphicsPipeline.h"
#include "src/graphics_pipeline/special_pipelines/ForwardRenderedGraphicsPipeline.h"

VertexBuffer createVertexBuffer(VulkanContext &context, const char *path) {
    VertexBuffer buffer(*context.allocator);
    OBJFile file = OBJFile::fromFilePath(path);
    auto raw = file.createVulkanBuffer();
    buffer.vertices = std::move(raw);
//    buffer.vertices.push_back(Vertex::positionOnly({0, 0, 0}));
//    buffer.vertices.push_back(Vertex::positionOnly({1, 0, 0}));
//    buffer.vertices.push_back(Vertex::positionOnly({0, 1, 0}));

    return buffer;
}


struct ImageViewPair {
    VulkanAllocator::VulkanImageAllocation image;
    vk::ImageView imageView;
};

DescriptorSampler createSampler(VulkanContext &context);

using UBO = ForwardRenderedGraphicsPipeline::UBO;

GraphicsPipeline createShadowPipeline(VulkanContext& context, DescriptorSet* descriptorSet) {
    GraphicsRenderPass renderPass(context);
    renderPass.useColor = false;
    renderPass.storeDepth = true;

    vk::SubpassDependency dependency {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = {};
    dependency.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;


    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo {};
    pipelineRasterizationStateCreateInfo.setCullMode(vk::CullModeFlagBits::eBack);
    pipelineRasterizationStateCreateInfo.setRasterizerDiscardEnable(false);
    pipelineRasterizationStateCreateInfo.setLineWidth(1.0f);
    pipelineRasterizationStateCreateInfo.setPolygonMode(vk::PolygonMode::eFill);
    pipelineRasterizationStateCreateInfo.setFrontFace(vk::FrontFace::eClockwise);
    pipelineRasterizationStateCreateInfo.setDepthBiasEnable(false);

    renderPass.init();
    auto shaders = GraphicsShaders(context, "shaders/basic.vert", "shaders/shadow.frag");
    auto builder = GraphicsPipelineBuilder(context, shaders, renderPass);
    builder.setExtent({2048, 2048});
    builder.setPipelineRasterizationStateCreateInfo(pipelineRasterizationStateCreateInfo);
    builder.addDepthImage();


    if (descriptorSet != nullptr) {
        builder.descriptorSets.push_back(descriptorSet);
    }
    auto pipeline = builder.buildGraphicsPipeline();

    return std::move(pipeline);
}

VulkanAllocator::VulkanBufferAllocation createUniformBuffer(VulkanContext& context) {
    vk::BufferCreateInfo bufferCreateInfo {};
    bufferCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
    bufferCreateInfo.setSize(sizeof (UBO));
    bufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);

    VkBufferCreateInfo rawCreateInfo = bufferCreateInfo;

    VulkanAllocator::VulkanBufferAllocation buffer;
    context.allocator->allocateBuffer(&rawCreateInfo, VMA_MEMORY_USAGE_AUTO, &buffer);
    return buffer;
}

DescriptorSet createUniformBindings(VulkanContext& context, DescriptorSampler& descriptorSampler) {
    vk::DescriptorSetLayoutBinding bufferBinding {};
    bufferBinding.setBinding(0);
    bufferBinding.setDescriptorCount(1);
    bufferBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
    bufferBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);


    vk::DescriptorSetLayoutBinding imageBinding {};
    imageBinding.setBinding(1);
    imageBinding.setDescriptorCount(1);
    imageBinding.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    imageBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
    imageBinding.setImmutableSamplers(*descriptorSampler.getSampler());

    vk::DescriptorSetLayoutBinding dynamicBufferBinding {};
    dynamicBufferBinding.setBinding(2);
    dynamicBufferBinding.setDescriptorCount(1);
    dynamicBufferBinding.setDescriptorType(vk::DescriptorType::eUniformBufferDynamic);
    dynamicBufferBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    DescriptorSet descriptorSet(context, {bufferBinding, imageBinding, dynamicBufferBinding});
    descriptorSet.buildDescriptor();

    return descriptorSet;
}

DescriptorSampler createSampler(VulkanContext &context) {
    auto descriptorSampler = DescriptorSampler(context);
    descriptorSampler.buildSampler();
    return descriptorSampler;
}

UBO initialBuffer() {
    glm::mat4 proj = glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 100.0);
    glm::mat4 view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0));
    view = glm::translate(view, glm::vec3(0, 0, -4));
    auto model = glm::identity<glm::mat4>();

    return {
        proj,
        view,
        model,
    };
}

int main() {
    VulkanContext context {};
    context.initVulkan();
    DescriptorSampler sampler = createSampler(context);
    auto descriptor = createUniformBindings(context, sampler);
    auto forward_descriptor = createUniformBindings(context, sampler);
    GraphicsRenderPass renderPass(context);
    renderPass.init();

//    GraphicsPipeline pipeline(context, std::move(renderPass));
//    pipeline.descriptorSet = &descriptor;
//    for (auto& swapChainImage: context.swapChainImageViews) {
//        pipeline.targetImageViews.push_back(*swapChainImage);
//    }
//    pipeline.init();
    auto shaders = GraphicsShaders(context, "shaders/basic.vert", "shaders/basic.frag");
    GraphicsPipelineBuilder builder = GraphicsPipelineBuilder(context, shaders, renderPass);
    builder.targetImageViews = {};
    for (auto& targetSwapchainImageView: context.swapChainImageViews) {
        builder.targetImageViews.push_back(*targetSwapchainImageView);
    }
    builder.descriptorSets = {&forward_descriptor};
    builder.addDepthImage();
    auto modelPipeline = ModelBufferGraphicsPipeline(builder.buildGraphicsPipeline(), 16);
    modelPipeline.descriptorSet = &forward_descriptor;
    auto pipeline = ForwardRenderedGraphicsPipeline(modelPipeline);
    auto shadowModelPipeline = ModelBufferGraphicsPipeline(createShadowPipeline(context, &descriptor), modelPipeline.modelBuffer);
    shadowModelPipeline.descriptorSet = &descriptor;
    auto shadow_pipeline = ShadowGraphicsPipeline(shadowModelPipeline);
    pipeline.descriptorSet = &forward_descriptor;
    shadow_pipeline.descriptorSet = &descriptor;
    GraphicsCommandBuffer commandBuffer(context);

    commandBuffer.pipelines.push_back(&shadow_pipeline);
    commandBuffer.pipelines.push_back(&pipeline);

    sampler.targetImageView = *shadow_pipeline.getPipeline().ownedImages[0].imageView;
    sampler.updateSampler(forward_descriptor, 1);


    commandBuffer.init();
    auto vertexbuffer1 = createVertexBuffer(context, "./models/super_backpack.obj");
    vertexbuffer1.init();
    commandBuffer.vertexBuffers.push_back(&vertexbuffer1);
    auto vertexbuffer2 = createVertexBuffer(context, "./models/floor.obj");
    vertexbuffer2.init();
    commandBuffer.vertexBuffers.push_back(&vertexbuffer2);
    modelPipeline.modelBuffer->updateBuffer({
        glm::identity<glm::mat4>(),
        glm::vec4(0.8f, 0.1f, 0.26f, 0.0f)
    }, 0);
    modelPipeline.modelBuffer->updateBuffer({
        glm::translate(glm::scale(glm::identity<glm::mat4>(), glm::vec3(2.3, 0.3, 2.3)), glm::vec3(0, -8, 0)),
        glm::vec4(127.0f, 255.0f, 212.0f, 0.0f) / glm::vec4(256.f),
    }, 1);

    auto initial_ubo = initialBuffer();

    vk::ImageMemoryBarrier imageMemoryBarrier {};
    auto depthImage = shadow_pipeline.getPipeline().ownedImages[0].imageAllocation.image;
    imageMemoryBarrier.setImage(depthImage);
    imageMemoryBarrier.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1));
    imageMemoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite);
    imageMemoryBarrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
    imageMemoryBarrier.setOldLayout(vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal);
    imageMemoryBarrier.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    GraphicsCommandBuffer::Dependency dependency {};
    dependency.imageBarrier = imageMemoryBarrier;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eLateFragmentTests;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;

    commandBuffer.dependencies.push_back(dependency);

    commandBuffer.bindings.push_back({
        &descriptor,
        &shadow_pipeline.getPipeline().getPipelineLayout(),
    });
    commandBuffer.bindings.push_back({
         &forward_descriptor,
         &pipeline.getPipeline().getPipelineLayout(),
    });
//    commandBuffer.layout = layout;
//    commandBuffer.descriptorSet = &descriptor;
//    commandBuffer.bindDescriptorSet(descriptor, commandBuffer.pipelines[0].getPipelineLayout(), 0);

    glfwShowWindow(context.window);
    auto last_time = glfwGetTime();
    float speed = 0.4;
    auto position = glm::zero<glm::vec3>();
    position.z = -5.f;
    auto rotation = glm::zero<glm::vec3>();
    while (!glfwWindowShouldClose(context.window)) {
        glfwPollEvents();
        commandBuffer.beginSwapchainRender();
        auto delta = glfwGetTime() - last_time;

        pipeline.ubo.lightProjView = shadow_pipeline.lightUBO.proj * shadow_pipeline.lightUBO.view;
        if (glfwGetKey(context.window, GLFW_KEY_W)) {
            position += glm::vec3(0, 0, speed * delta);
        }

        if (glfwGetKey(context.window, GLFW_KEY_S)) {
            position += glm::vec3(0, 0, -speed * delta);
        }

        if (glfwGetKey(context.window, GLFW_KEY_D)) {
            position += glm::vec3(-speed * delta, 0, 0);
        }

        if (glfwGetKey(context.window, GLFW_KEY_A)) {
            position += glm::vec3(speed * delta, 0, 0);
        }

        if (glfwGetKey(context.window, GLFW_KEY_Q)) {
            rotation += glm::vec3(0, -1.0f * (float)delta, 0);
        }

        if (glfwGetKey(context.window, GLFW_KEY_E)) {
            rotation += glm::vec3(0, 1.0f * (float)delta, 0);
        }

        pipeline.ubo.view = glm::translate(glm::rotate(glm::identity<glm::mat4>(), rotation.y, glm::vec3(0, 1, 0)), position);

        sampler.updateSampler(forward_descriptor, 1);
        last_time = glfwGetTime();
        commandBuffer.finishSwapchainRender();
    }


    context.device.waitIdle();
    modelPipeline.modelBuffer->destroy();
    shadow_pipeline.getPipeline().destroy();
    pipeline.getPipeline().destroy();
    commandBuffer.destroy();
    context.cleanup();
    return 0;
}
