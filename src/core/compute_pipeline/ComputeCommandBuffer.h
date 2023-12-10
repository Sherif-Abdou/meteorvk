//
// Created by Sherif Abdou on 11/3/23.
//

#ifndef VULKAN_ENGINE_COMPUTECOMMANDBUFFER_H
#define VULKAN_ENGINE_COMPUTECOMMANDBUFFER_H

#include "../VulkanContext.h"
#include "ComputePipeline.h"

class ComputeCommandBuffer {
public:
    static constexpr uint32_t FRAMES_IN_FLIGHT = 2;
    explicit ComputeCommandBuffer(VulkanContext *context);
    VulkanContext* context;
    void init();
    void begin();
    void bindAndDispatch(ComputePipeline& pipeline, std::optional<DescriptorSet*> set);
    void end();
    void submit();
    void nextFrame();
    void bindDescriptorSet(DescriptorSet&, vk::raii::PipelineLayout&);
    uint32_t getCurrentFrame() const;
    vk::Semaphore getCurrentSignalSemaphore();
private:
    vk::raii::CommandPool commandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> commandBuffer {};
    std::vector<vk::raii::Fence> inFlightFences {};
    std::vector<vk::raii::Semaphore> signalSemaphores {};
    void createCommandPool();
    void createCommandBuffer();
    void createSyncObjects();
    uint32_t currentFrame = 0;

};


#endif //VULKAN_ENGINE_COMPUTECOMMANDBUFFER_H
