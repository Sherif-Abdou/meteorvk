#include "NewRenderer.h"

void NewRenderer::run() {
    buildDescriptorLayouts();
    auto vertex = createVertexBuffer(context, "./models/car.obj");
    vertex.init();

    pipeline =
        buildForwardGraphicsPipeline();

    GraphicsCommandBuffer command_buffer(context);
    command_buffer.init();


    RenderableChain render_chain(context, descriptorManager.get());

    render_chain.addRenderable({
            pipeline.get(),
            {},
            });
    command_buffer.vertexBuffers = { &vertex };

    render_chain.applyToCommandBuffer(&command_buffer);
    // command_buffer.pipelines = { pipeline.get() };
    // command_buffer.bindings = {{
    //         descriptorManager->getDescriptorFor(NewDescriptorManager::BindingUpdateRate::Frame),
    //         &pipeline->getGraphicsPipeline().getPipelineLayout(),
    //         0,
    // }};

    glfwShowWindow(context->window);

    auto last_time = glfwGetTime();
    while (!glfwWindowShouldClose(context->window)) {
        glfwPollEvents();
        auto current_time = glfwGetTime();
        command_buffer.beginSwapchainRender();

        tick(current_time - last_time);

        command_buffer.finishSwapchainRender();
        last_time = current_time;
    }

    context->device.waitIdle();

    command_buffer.destroy();
    pipeline->destroy();

    model_buffer->destroy();
    delete model_buffer;
}


std::unique_ptr<ForwardRenderedGraphicsPipeline> NewRenderer::buildForwardGraphicsPipeline() {
    GraphicsPipelineBuilder2 builder(context);
    builder.descriptorManager = descriptorManager.get();
    builder.options.useDepth = true;
    builder.options.vertexShaderPath = "./shaders/new_renderer.vert";
    builder.options.fragmentShaderPath = "./shaders/new_renderer.frag";
    builder.options.multisampling = true;
    builder.options.imageSource = GraphicsPipelineBuilder2::ImageSource::Swapchain;

    GraphicsPipeline pipeline = builder.build();

    auto model_pipeline = std::make_unique<ModelBufferGraphicsPipeline>(std::move(pipeline), 8);
    model_pipeline->modelBuffer->updateBuffer({
            glm::identity<glm::mat4>(),
            Material(glm::vec4(0.3,0.1,0.7,1)),
            }, 0);
    model_pipeline->descriptors = descriptorManager.get();

    model_buffer = model_pipeline->modelBuffer;

    auto forward_pipeline = std::make_unique<ForwardRenderedGraphicsPipeline>(std::move(model_pipeline));
    forward_pipeline->descriptors = descriptorManager.get();

    auto view = glm::identity<glm::mat4>();
    view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
    forward_pipeline->ubo.proj = glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 20.0);

    return forward_pipeline;
}

void NewRenderer::buildDescriptorLayouts() {
    descriptorManager = std::make_unique<NewDescriptorManager>(context);

    vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;

    descriptorManager->addLayoutBinding(ForwardRenderedGraphicsPipeline::FORWARD_UBO_NAME, 
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer),
            NewDescriptorManager::BindingUpdateRate::Frame
            );
    descriptorManager->addLayoutBinding(ModelBufferGraphicsPipeline::MODEL_BUFFER_DESCRIPTOR_NAME, 
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic),
            NewDescriptorManager::BindingUpdateRate::Frame
            );
}

VertexBuffer NewRenderer::createVertexBuffer(VulkanContext *context, const char *path) {
    VertexBuffer buffer(context, true);
    NewOBJFile file = NewOBJFile::fromFilePath(path);
    auto raw = file.exportIndexedBuffer();

    buffer.vertices = std::move(raw.vertices);
    buffer.indices = std::move(raw.indices);
    return buffer;
}

void NewRenderer::tick(double elapsed) {
    if (glfwGetKey(context->window, GLFW_KEY_W)) {
        pipeline->ubo.view = glm::translate(pipeline->ubo.view, glm::vec3(0.0f,0.0f, sensitivity * elapsed));
    }
    if (glfwGetKey(context->window, GLFW_KEY_S)) {
        pipeline->ubo.view = glm::translate(pipeline->ubo.view, glm::vec3(0.0f,0.0f, -sensitivity * elapsed));
    }
    if (glfwGetKey(context->window, GLFW_KEY_A)) {
        pipeline->ubo.view = glm::translate(pipeline->ubo.view, glm::vec3(sensitivity * elapsed,0.0f, 0.0f));
    }
    if (glfwGetKey(context->window, GLFW_KEY_D)) {
        pipeline->ubo.view = glm::translate(pipeline->ubo.view, glm::vec3(-sensitivity * elapsed,0.0f, 0.0f));
    }
}
