#include "LightBuffer.h"
#include <memory>

LightBuffer::LightBuffer(VulkanContext* context): context(context) {
    ubo = std::make_unique<UBO>();
    ubo->light_count = 0;

    ubo_buffer = std::make_unique<UniformBuffer<UBO>>(context);
    ubo_buffer->allocateBuffer();
}

void LightBuffer::updateDescriptor(DescriptorSet* descriptor, uint32_t binding) {
    ubo_buffer->updateBuffer(*ubo);

    ubo_buffer->writeToDescriptor(*descriptor, binding);
}

void LightBuffer::updateDescriptor(NewDescriptorManager* descriptorManager) {

    this->updateDescriptor(descriptorManager->getDescriptorFor(LIGHT_UBO_NAME), descriptorManager->getBindingOf(LIGHT_UBO_NAME));
}

uint32_t LightBuffer::addLight(glm::vec3 position, glm::mat4 pov, int32_t intensity) {
    uint32_t index = ubo->light_count;
    ubo->light_positions[index] = glm::vec4(position, intensity);
    ubo->light_povs[index] = pov;
    ubo->light_count += 1;
    ubo_buffer->updateBuffer(*ubo);
    return index;
}


void LightBuffer::addLayoutBinding(NewDescriptorManager* descriptorManager) {
    descriptorManager->addLayoutBindingForFrame(
            LIGHT_UBO_NAME,
            vk::DescriptorSetLayoutBinding()
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            );
}

LightBuffer::~LightBuffer() {
    ubo_buffer->destroy(); 
}
