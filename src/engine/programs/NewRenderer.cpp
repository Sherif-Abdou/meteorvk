#include "NewRenderer.h"
#include "../../core/shared_pipeline/PipelineBarrierBuilder.h"

void NewRenderer::run() {
    buildDescriptorLayouts();
    auto vertex = createVertexBuffer(context, "./models/car.obj");
    vertex.init();

    pipeline =
        buildForwardGraphicsPipeline();

    GraphicsCommandBuffer command_buffer(context);
    command_buffer.init();


    RenderableChain render_chain(context, descriptorManager.get());

    depth_pipeline =
        buildDepthOnlyPipeline();

    ssao_pipeline =
        buildSSAOGraphicsPipeline(depth_pipeline->getDepthImageView());

    auto depth_image_barrier = PipelineBarrierBuilder();
    depth_image_barrier
            .waitFor(vk::PipelineStageFlagBits2::eLateFragmentTests | vk::PipelineStageFlagBits2::eEarlyFragmentTests)
            .whichUses(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
            .beforeDoing(vk::PipelineStageFlagBits2::eFragmentShader)
            .whichUses(vk::AccessFlagBits2::eShaderRead)
            .forImage(depth_pipeline->getDepthImage(), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1))
            .withInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
            .withFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    auto occlusion_image_barrier = PipelineBarrierBuilder();
    occlusion_image_barrier
            .waitFor(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
            .whichUses(vk::AccessFlagBits2::eColorAttachmentWrite)
            .beforeDoing(vk::PipelineStageFlagBits2::eFragmentShader)
            .whichUses(vk::AccessFlagBits2::eShaderRead)
            .forImage(ssao_pipeline->getOcclusionImage(), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
            .withInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .withFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    render_chain.addRenderable({
            depth_pipeline.get(),
            &depth_pipeline.get()->getPipeline().getPipelineLayout(),
            });
    render_chain.addRenderable({
            ssao_pipeline.get(),
            &ssao_pipeline->getPipeline().getPipelineLayout(),
            // {depth_image_barrier.build()},
            });
    render_chain.addRenderable({
            pipeline.get(),
            &pipeline.get()->getGraphicsPipeline().getPipelineLayout(),
            {occlusion_image_barrier.build()}
            });

    command_buffer.vertexBuffers = { &vertex };

    render_chain.applyToCommandBuffer(&command_buffer);

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
    depth_pipeline->destroy();
    ssao_pipeline->destroy();

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
    auto model_mat = glm::identity<glm::mat4>();
    model_mat = glm::rotate(model_mat, glm::radians(45.f), glm::vec3(0.f, 1.f, 0.f));
    model_mat = glm::translate(model_mat, glm::vec3(0.f, -1.f, 0.f));
    model_pipeline->modelBuffer->updateBuffer({
            model_mat,
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
    descriptorManager->addLayoutBinding(SSAOGraphicsPipeline::SSAO_DEPTH_NAME, 
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler),
            NewDescriptorManager::BindingUpdateRate::Frame
            );
    descriptorManager->addLayoutBinding(SSAOGraphicsPipeline::SSAO_NOISE_NAME, 
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler),
            NewDescriptorManager::BindingUpdateRate::Frame
            );
    descriptorManager->addLayoutBinding("occlusion_map", 
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler),
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

    occlusion_sampler->updateSampler(*descriptorManager->getDescriptorFor("occlusion_map"), descriptorManager->getBindingOf("occlusion_map"));

    depth_pipeline->getUBO().ubo.view = pipeline->ubo.view;
    ssao_pipeline->ubo->view = pipeline->ubo.view;
}

std::unique_ptr<DepthOnlyPipeline> NewRenderer::buildDepthOnlyPipeline() {
    GraphicsPipelineBuilder2 builder(context, descriptorManager.get());

    builder.options.useDepth = true;
    builder.options.shouldStoreDepth = true;
    builder.options.multisampling = false;
    builder.options.imageSource = GraphicsPipelineBuilder2::ImageSource::Depth;
    builder.options.vertexShaderPath = "shaders/new_renderer.vert";
    builder.options.fragmentShaderPath = "shaders/shadow.frag";

    GraphicsPipeline pipeline = builder.build();

    auto model_pipeline = std::make_unique<ModelBufferGraphicsPipeline>(std::move(pipeline), model_buffer);
    model_pipeline->descriptors = descriptorManager.get();

    auto forward_pipeline = std::make_unique<ForwardRenderedGraphicsPipeline>(std::move(model_pipeline));
    forward_pipeline->descriptors = descriptorManager.get();

    auto view = glm::identity<glm::mat4>();
    view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
    forward_pipeline->ubo.proj = glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 20.0);
    forward_pipeline->ubo.view = view;

    auto depth_pipeline = std::make_unique<DepthOnlyPipeline>(std::move(forward_pipeline));

    return depth_pipeline;
}

std::unique_ptr<SSAOGraphicsPipeline> NewRenderer::buildSSAOGraphicsPipeline(vk::ImageView depth_image_view) {
    GraphicsPipelineBuilder2 builder(context, descriptorManager.get());

    builder.options.useDepth = true;
    builder.options.shouldStoreDepth = false;
    builder.options.multisampling = false;
    builder.options.imageSource = GraphicsPipelineBuilder2::ImageSource::Custom;
    builder.options.vertexShaderPath = "shaders/new_renderer.vert";
    builder.options.fragmentShaderPath = "shaders/ssao.frag";
    builder.options.format = vk::Format::eR16Sfloat;

    auto pipeline = builder.build();

    auto model_pipeline = std::make_unique<ModelBufferGraphicsPipeline>(std::move(pipeline), model_buffer);
    model_pipeline->descriptors = descriptorManager.get();

    depth_sampler = std::make_unique<CombinedDescriptorSampler>(context);
    depth_sampler->targetImageView = depth_image_view;
    depth_sampler->buildSampler();


    auto ssao_pipeline = std::make_unique<SSAOGraphicsPipeline>(context, std::move(model_pipeline), nullptr);
    ssao_pipeline->setDepthSampler(depth_sampler.get());
    ssao_pipeline->descriptors = descriptorManager.get();
    ssao_pipeline->init();

    auto view = glm::identity<glm::mat4>();
    view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
    ssao_pipeline->ubo->proj = glm::perspective(glm::radians(90.0), 1920.0 / 1080.0, 0.1, 20.0);
    ssao_pipeline->ubo->view = view;

    occlusion_sampler = std::make_unique<CombinedDescriptorSampler>(context); 
    occlusion_sampler->targetImageView = ssao_pipeline->getOcclusionImageView();
    occlusion_sampler->targetImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    occlusion_sampler->buildSampler();

    return ssao_pipeline;
}

