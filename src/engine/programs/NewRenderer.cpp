#include "NewRenderer.h"
#include "core/shared_pipeline/PipelineBarrierBuilder.h"
#include "core/storage/DescriptorSampler.h"
#include "core_v2/GraphicsPipelineBuilder2.h"
#include "core/graphics_pipeline/GraphicsCommandBuffer.h"
#include "core_v2/render_chain/RenderableChain.h"
#include "core/storage/NewOBJFile.h"
#include "engine/special_pipelines/ModelBufferGraphicsPipeline.h"
#include "engine/storage/MTLFile.h"
#include "engine/storage/ModelBuffer.h"
#include "engine/storage/VertexBufferLoader.h"
#include "vulkan/vulkan_handles.hpp"

#include <chrono>

void NewRenderer::run() {
    auto start = std::chrono::system_clock::now();
    buildDescriptorLayouts();
    buildLighting();
    buildTexturing();

    pipeline =
        buildForwardGraphicsPipeline();

    std::array<const char*, 4> names = {"./models/car.obj", "./models/super_backpack.obj", "./models/floor.obj", "./models/non_triangled_senna.obj"};
    loader =
        std::make_unique<VertexBufferLoader>(context, model_buffer, descriptorManager.get());

    loader->addModel("car", "./models/car.obj", glm::identity<glm::mat4>());
    loader->addModel("f", "./models/floor.obj", glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, -3, 0)));
    loader->addModel("senna", "./models/non_triangled_senna.obj", glm::translate(glm::identity<glm::mat4>(), glm::vec3(3, 0,0)));

    // loader->addModel("senna2", "./models/non_triangled_senna.obj", glm::translate(glm::identity<glm::mat4>(), glm::vec3(-3, 0,0)));

    loader->addModel("back", "./models/super_backpack.obj", glm::translate(glm::identity<glm::mat4>(), glm::vec3(-3, 0,0)), true);

    GraphicsCommandBuffer command_buffer(context);
    command_buffer.init();

    command_buffer.descriptorManager = descriptorManager.get();

    RenderableChain render_chain(context, descriptorManager.get());


    shadow_pipeline = 
        buildShadowGraphicsPipeline();

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
    auto shadow_map_barrier = PipelineBarrierBuilder()
        .waitFor(vk::PipelineStageFlagBits2::eLateFragmentTests | vk::PipelineStageFlagBits2::eEarlyFragmentTests)
        .whichUses(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
        .beforeDoing(vk::PipelineStageFlagBits2::eFragmentShader)
        .whichUses(vk::AccessFlagBits2::eShaderRead)
        .forImage(shadow_pipeline->getDepthImage(), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1))
        .withInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .withFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    render_chain.addRenderable({
            shadow_pipeline.get(),
            &shadow_pipeline.get()->getPipeline().getPipelineLayout(),
            });
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
            {occlusion_image_barrier.build(), shadow_map_barrier.build()}
            });

    /*
    auto vertex = createVertexBuffer(context, "./models/car.obj");
    vertex.init();
    auto vertex2 = createVertexBuffer(context, "./models/super_backpack.obj");
    vertex2.init();
    auto ground = createVertexBuffer(context, "./models/floor.obj");
    ground.init();
    */


    render_chain.applyToCommandBuffer(&command_buffer);

    loader->waitForAll();

    texture_container->primitiveApplyToDescriptorSet(loader->getDescriptorForModel("back"), descriptorManager.get(), 2);

    texture_container->primitiveApplyToDescriptorSet(loader->getDescriptorForModel("f"), descriptorManager.get(), 1);

    texture_container->primitiveApplyToDescriptorSet(loader->getDescriptorForModel("car"), descriptorManager.get(), 0);

    texture_container->primitiveApplyToDescriptorSet(loader->getDescriptorForModel("senna"), descriptorManager.get(), 0);

    loader->attachToCommandBuffer(&command_buffer.vertexBuffers);
    auto end = std::chrono::system_clock::now();

    std::cout << end - start << "\n";
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
    shadow_pipeline->destroy();
    depth_pipeline->destroy();
    ssao_pipeline->destroy();

    model_buffer->destroy();
}


std::unique_ptr<ForwardRenderedGraphicsPipeline> NewRenderer::buildForwardGraphicsPipeline() {
    GraphicsPipelineBuilder2 builder(context, nullptr, "forward_pipeline");
    builder.descriptorManager = descriptorManager.get();
    builder.options.useDepth = true;
    builder.options.vertexShaderPath = "./shaders/new_renderer.vert";
    builder.options.fragmentShaderPath = "./shaders/new_renderer.frag";
    builder.options.multisampling = false;
    builder.options.imageSource = GraphicsPipelineBuilder2::ImageSource::Swapchain;

    GraphicsPipeline pipeline = builder.build();

    model_buffer = new ModelBuffer(context, 2048);
    auto model_pipeline = std::make_unique<ModelBufferGraphicsPipeline>(std::move(pipeline), model_buffer);
    /*
    auto model_mat = glm::identity<glm::mat4>();
    model_mat = glm::rotate(model_mat, glm::radians(45.f), glm::vec3(0.f, 1.f, 0.f));
    model_mat = glm::translate(model_mat, glm::vec3(0.f, -1.f, 0.f));
    model_pipeline->modelBuffer->updateBuffer({
            model_mat,
            Material(),
            }, 0);
    model_mat = glm::translate(model_mat, glm::vec3(3.0f, 0.0f, 3.0f));
    model_pipeline->modelBuffer->updateBuffer({
            model_mat,
            Material(),
            }, 1);

    auto ground_model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, -3, 0));
    model_pipeline->modelBuffer->updateBuffer({
            ground_model,
            Material(),
            }, 2);
    auto senna_model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(5, -3, 0));
    model_pipeline->modelBuffer->updateBuffer({
            senna_model,
            Material(),
            }, 3);
    */

    model_pipeline->descriptors = descriptorManager.get();

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

    descriptorManager->addLayoutBindingForFrame(ForwardRenderedGraphicsPipeline::FORWARD_UBO_NAME, 
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            );

    descriptorManager->addLayoutBindingForFrame(ModelBufferGraphicsPipeline::MODEL_BUFFER_DESCRIPTOR_NAME, 
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
            );

    descriptorManager->addLayoutBindingForPipeline("shadow_ubo", "shadow_pipeline",
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            );
    descriptorManager->addLayoutBindingForPipeline(SSAOGraphicsPipeline::SSAO_DEPTH_NAME, "ssao_pipeline",
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            );

    descriptorManager->addLayoutBindingForPipeline(SSAOGraphicsPipeline::SSAO_NOISE_NAME, "ssao_pipeline",
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            );

    descriptorManager->addLayoutBindingForFrame("occlusion_map", 
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            );
    descriptorManager->addLayoutBindingForFrame("shadow_map", 
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(stages)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            );

    descriptorManager->addModelLayoutBinding("model_sampler",
            vk::DescriptorSetLayoutBinding()
            .setStageFlags(stages)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eSampler));
    descriptorManager->addModelLayoutBinding("model_texture",
            vk::DescriptorSetLayoutBinding()
            .setStageFlags(stages)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eSampledImage));
    descriptorManager->addModelLayoutBinding("model_info",
            vk::DescriptorSetLayoutBinding()
            .setStageFlags(stages)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer));

    TextureDescriptorSet::attachLayoutToDescriptorManager(descriptorManager.get());
}

VertexBuffer NewRenderer::createVertexBuffer(VulkanContext *context, const char *path) {
    VertexBuffer buffer(context, true);
    NewOBJFile file = NewOBJFile::fromFilePath(path);
    auto raw = file.exportIndexedBuffer();

    buffer.vertices = std::move(raw.vertices);
    buffer.indices = std::move(raw.indices);

    if (!file.getMaterialPaths().empty() && texture_container != nullptr) {
      std::unordered_map<uint32_t, std::string> index_map =
          file.getMTLIndexMap();

      for (auto [mtl_index, mtl_path] : index_map) {
          auto mtl_file = MTLFile::fromFilePath(mtl_path);
          std::unordered_map<std::string, RenderMaterial> mtl_materials =
              mtl_file.getRenderMaterials();
          for (auto [local_material_name, local_material]: mtl_materials) {
              texture_container->addMaterial(local_material);
          }
      }
    }
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
    if (glfwGetKey(context->window, GLFW_KEY_SPACE)) {
        pipeline->ubo.view = glm::translate(pipeline->ubo.view, glm::vec3(0.0f,-sensitivity * elapsed, 0.0f));
    }
    if (glfwGetKey(context->window, GLFW_KEY_Z)) {
        pipeline->ubo.view = glm::translate(pipeline->ubo.view, glm::vec3(0.0f,sensitivity * elapsed, 0.0f));
    }

    occlusion_sampler->updateSampler(*descriptorManager->getDescriptorFor("occlusion_map"), descriptorManager->getBindingOf("occlusion_map"));
    shadow_sampler->updateSampler(*descriptorManager->getDescriptorFor("shadow_map"), descriptorManager->getBindingOf("shadow_map"));


    glm::mat4 shadow_proj = glm::ortho(-20.f, 20.f, -20.f, 20.f, 0.1f, 20.f);

    shadow_pipeline->lightUBO.view = glm::lookAt(glm::vec3(0, 5, -3), glm::vec3(0,0,0), glm::vec3(0,0, 1));
    pipeline->ubo.lightProjView = shadow_pipeline->exportLightProjView();

    light_buffer->updateDescriptor(descriptorManager.get());

    texture_container->copyMaterialsTo(texture_descriptor.get());


    texture_descriptor->uploadMaterialList();
}

std::unique_ptr<DepthOnlyPipeline> NewRenderer::buildDepthOnlyPipeline() {
    GraphicsPipelineBuilder2 builder(context, descriptorManager.get(), "ssao_depth_pipeline");

    builder.options.useDepth = true;
    builder.options.shouldStoreDepth = true;
    builder.options.multisampling = false;
    builder.options.imageSource = GraphicsPipelineBuilder2::ImageSource::Depth;
    builder.options.extent = {1280, 720};
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
    GraphicsPipelineBuilder2 builder(context, descriptorManager.get(), "ssao_pipeline");

    builder.options.useDepth = true;
    builder.options.shouldStoreDepth = false;
    builder.options.multisampling = false;
    builder.options.imageSource = GraphicsPipelineBuilder2::ImageSource::Custom;
    builder.options.vertexShaderPath = "shaders/new_renderer.vert";
    builder.options.fragmentShaderPath = "shaders/ssao.frag";
    builder.options.extent = {1280, 720};
    builder.options.format = vk::Format::eR16Sfloat;
    builder.options.pipeline_name = "ssao_pipeline";

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

std::unique_ptr<ShadowGraphicsPipeline> NewRenderer::buildShadowGraphicsPipeline() {
    GraphicsPipelineBuilder2 builder(context, descriptorManager.get(), "shadow_pipeline");

    builder.options.useDepth = true;
    builder.options.shouldStoreDepth = true;
    builder.options.multisampling = false;
    builder.options.imageSource = GraphicsPipelineBuilder2::ImageSource::Depth;
    builder.options.extent = {2048, 2048};
    builder.options.vertexShaderPath = "shaders/shadow.vert";
    builder.options.fragmentShaderPath = "shaders/shadow.frag";
    builder.options.pipeline_name = "shadow_pipeline";

    GraphicsPipeline pipeline = builder.build();

    auto model_pipeline = std::make_unique<ModelBufferGraphicsPipeline>(std::move(pipeline), model_buffer);
    model_pipeline->descriptors = descriptorManager.get();

    auto shadow_pipeline = std::make_unique<ShadowGraphicsPipeline>(std::move(model_pipeline));

    shadow_sampler = std::make_unique<CombinedDescriptorSampler>(context);
    shadow_sampler->targetImageView = shadow_pipeline->getDepthImageView();
    shadow_sampler->targetImageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    shadow_sampler->buildSampler();

    return std::move(shadow_pipeline);
}


void NewRenderer::buildLighting() {
    light_buffer = std::make_unique<LightBuffer>(context);
    light_buffer->addLayoutBinding(descriptorManager.get());
    

    light_buffer->addLight(glm::vec3(0, 5, -3), 
            glm::lookAt(glm::vec3(0, 2, 0), glm::vec3(0,0,0), glm::vec3(0, 0, 1)), 0.f);

    light_buffer->addLight(glm::vec3(2, 2, 0), 
            glm::lookAt(glm::vec3(0, 5, 0), glm::vec3(0,0,0), glm::vec3(0, 0, 1)));

    light_buffer->addLight(glm::vec3(-2, 2, 0), 
            glm::lookAt(glm::vec3(0, 5, 0), glm::vec3(0,0,0), glm::vec3(0, 0, 1)));
}


void NewRenderer::buildTexturing() {
    texture_descriptor = std::make_unique<TextureDescriptorSet>(context);
    texture_descriptor->descriptorManager = descriptorManager.get();

    texture_container = std::make_unique<TextureContainer>(context);
    texture_container->addTextureFromPath("./textures/red.jpeg");
    texture_container->addTextureFromPath("./textures/img.png");
    texture_container->addTextureFromPath("./textures/1001_albedo.jpg");

    auto* sampler = new DescriptorSampler(context);
    sampler->buildSampler();

    texture_container->addSampler(sampler);
}


NewRenderer::~NewRenderer() {
    delete model_buffer;
}
