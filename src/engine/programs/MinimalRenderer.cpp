//
// Created by Sherif Abdou on 5/7/24.
//

#include "MinimalRenderer.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <thread>

struct UBO {
    glm::mat4 proj;
    glm::mat4 view;
};

constexpr const char* VERTEX_PATH = "./shaders/minimal.vert";
constexpr const char* FRAGMENT_PATH = "./shaders/minimal.frag";
constexpr const char* MODEL_PATH = "./models/car.obj";

void MinimalRenderer::run() {
    auto vertex = createVertexBuffer(context, MODEL_PATH);
    vertex.init();

    UniformBuffer<UBO> ubo(context);
    auto view = glm::identity<glm::mat4>();
    view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
    ubo.allocateBuffer();
    ubo.updateBuffer(UBO {
        glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 20.0),
        view
    });

    GraphicsPipeline pipeline = buildPipeline();

    std::cout << "Built!" << std::endl;

    auto command_buffer = GraphicsCommandBuffer(context);
    command_buffer.init();

    DescriptorSet* frame_descriptor = descriptorManager->getDescriptorFor(ubo_name);

    command_buffer.vertexBuffers = {&vertex};
    command_buffer.pipelines = {&pipeline};
    command_buffer.bindings = std::vector<GraphicsCommandBuffer::DescriptorPipelineBinding> {{
        frame_descriptor,
        &pipeline.getPipelineLayout(),
        0
    }};

    glfwShowWindow(context->window);

    while (!glfwWindowShouldClose(context->window)) {
        glfwPollEvents();
        command_buffer.beginSwapchainRender();
        ubo.writeToDescriptor(*descriptorManager->getDescriptorFor(ubo_name), descriptorManager->getBindingOf(ubo_name));
        command_buffer.finishSwapchainRender();
    }

    context->device.waitIdle();

    command_buffer.destroy();
    pipeline.destroy();
    buffer->destroy();
    delete buffer;
    ubo.destroy();
    delete descriptorManager;
}

VertexBuffer MinimalRenderer::createVertexBuffer(VulkanContext *context, const char *path) {
    VertexBuffer buffer(context, true);
    NewOBJFile file = NewOBJFile::fromFilePath(path);
    auto raw = file.exportIndexedBuffer();

    buffer.vertices = std::move(raw.vertices);
    buffer.indices = std::move(raw.indices);
    return buffer;
}

GraphicsPipeline MinimalRenderer::buildPipeline() {
    using namespace std::chrono_literals;



    buffer = new ModelBuffer(context, true);
    buffer->updateBuffer(ModelBuffer::PerModelBuffer {
        glm::identity<glm::mat4>(),
        Material(glm::zero<glm::vec4>())
    }, 0);


    descriptorManager = new NewDescriptorManager(context);

    GraphicsPipelineBuilder2 builder(context, descriptorManager);

    builder.options.useDepth = true;
    builder.options.multisampling = true;
    builder.options.vertexShaderPath = VERTEX_PATH;
    builder.options.fragmentShaderPath = FRAGMENT_PATH;
    builder.options.imageSource = GraphicsPipelineBuilder2::ImageSource::Swapchain;

    vk::ShaderStageFlags flag_bits = vk::ShaderStageFlagBits::eAllGraphics;
    builder.descriptorManager->addLayoutBinding(ubo_name,
                                         vk::DescriptorSetLayoutBinding().setDescriptorCount(1).setDescriptorType(vk::DescriptorType::eUniformBuffer).setStageFlags(flag_bits),
                                         NewDescriptorManager::BindingUpdateRate::Frame);

    GraphicsPipeline pipeline = builder.build();

    return pipeline;
}
