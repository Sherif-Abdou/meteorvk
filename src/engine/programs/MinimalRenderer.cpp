//
// Created by Sherif Abdou on 5/7/24.
//

#include "MinimalRenderer.h"
#include "../../core_v2/GraphicsPipelineBuilder2.h"
#include "../../core/graphics_pipeline/GraphicsCommandBuffer.h"
#include "../../core/storage/NewOBJFile.h"
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
    using namespace std::chrono_literals;

    const std::string ubo_name = "ubo";
    auto ubo = UniformBuffer<UBO>(context);
    auto view = glm::identity<glm::mat4>();
    view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
    ubo.allocateBuffer();
    ubo.updateBuffer(UBO {
        glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 20.0),
        view
    });

    auto vertex = createVertexBuffer(context, MODEL_PATH);
    vertex.init();
    vertex.updateVertexBuffer();

    buffer = new ModelBuffer(context, true);
    buffer->updateBuffer(ModelBuffer::PerModelBuffer {
        glm::identity<glm::mat4>(),
        Material(glm::zero<glm::vec4>())
    }, 0);

    GraphicsPipelineBuilder2 builder(context);
    builder.options.useDepth = true;
    builder.options.multisampling = true;
    builder.options.vertexShaderPath = VERTEX_PATH;
    builder.options.fragmentShaderPath = FRAGMENT_PATH;
    builder.options.imageSource = GraphicsPipelineBuilder2::ImageSource::Swapchain;

    vk::ShaderStageFlags flag_bits = vk::ShaderStageFlagBits::eAllGraphics;
    builder.descriptorManager.addBinding(ubo_name,
                                         vk::DescriptorSetLayoutBinding().setDescriptorCount(1).setDescriptorType(vk::DescriptorType::eUniformBuffer).setStageFlags(flag_bits),
                                         NewDescriptorManager::BindingUpdateRate::FRAME);

    auto pipeline = builder.build();

    std::cout << "Built?" << std::endl;

    auto command = GraphicsCommandBuffer(context);
    command.init();

    auto frame_descriptor = builder.descriptorManager.getDescriptorFor(ubo_name);

    command.vertexBuffers = {&vertex};
    command.pipelines = {&pipeline};
    command.bindings = {{
        frame_descriptor,
        &pipeline.getPipelineLayout(),
        0
    }};

    glfwShowWindow(context->window);

    while (!glfwWindowShouldClose(context->window)) {
        glfwPollEvents();
        command.beginSwapchainRender();
        ubo.writeToDescriptor(*builder.descriptorManager.getDescriptorFor(ubo_name), builder.descriptorManager.getBindingOf(ubo_name));
        command.finishSwapchainRender();
    }

    context->device.waitIdle();

    command.destroy();
    pipeline.destroy();
    buffer->destroy();
    ubo.destroy();
    delete buffer;
}

VertexBuffer MinimalRenderer::createVertexBuffer(VulkanContext *context, const char *path) {
    VertexBuffer buffer(context, true);
    NewOBJFile file = NewOBJFile::fromFilePath(path);
    auto raw = file.exportIndexedBuffer();
    /*
    if (!file.getMaterialPaths().empty()) {
        std::filesystem::path p = path;
        std::string sep = "/";
        auto new_path = p.parent_path().c_str() + sep + file.getMaterialPaths()[0];
        MTLFile new_file = MTLFile::fromFilePath(new_path);
        auto map = new_file.getRenderMaterials();
        auto mtl_indices = file.getMTLIndexMap();
        for (int i = 0; i < map.size(); i++) {
            auto material = map[mtl_indices[i]];
            textureContainer.addMaterial(material);
        }
    }
     */
    buffer.vertices = std::move(raw.vertices);
    buffer.indices = std::move(raw.indices);
    return buffer;
}
