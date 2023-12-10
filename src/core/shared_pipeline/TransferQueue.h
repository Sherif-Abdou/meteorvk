//
// Created by Sherif Abdou on 11/20/23.
//

#ifndef TRANSFERQUEUE_H
#define TRANSFERQUEUE_H
#include "PipelineBarrier.h"
#include "../VulkanContext.h"


class TransferQueue {
private:
    VulkanContext* context;
    vk::raii::CommandPool pool = nullptr;
    vk::raii::CommandBuffer command_buffer = nullptr;
    bool ran = false;
    void init();
public:
    explicit TransferQueue(VulkanContext* context): context(context) {
        init();
    };


    void begin();

    void applyBarrier(PipelineBarrier barrier);

    void copy(vk::Buffer src, vk::Buffer dst, vk::BufferCopy region);

    void copyBufferToImage(vk::Buffer src, vk::Image dst, vk::ImageLayout layout, vk::BufferImageCopy region);

    void submit();
};



#endif //TRANSFERQUEUE_H
