//
// Created by Sherif Abdou on 11/20/23.
//

#include "ImageTextureLoader.h"
#include "../../core/shared_pipeline/TransferQueue.h"
#include "../../core/shared_pipeline/PipelineBarrierBuilder.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../stbi/stbi_image.h"

VulkanAllocator::VulkanImageAllocation ImageTextureLoader::loadImageFromFile(const char* file_path) {
    stbi_set_flip_vertically_on_load(true);

    pixels = stbi_load(file_path, &width, &height, &channels, STBI_rgb_alpha);

    uint64_t size = width * height * 4;

    VkBufferCreateInfo staging_create_info { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    staging_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    staging_create_info.size = size;
    staging_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VkImageCreateInfo image_create_info { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };

    image_create_info.extent = VkExtent3D {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
    image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    context->allocator->allocateImage(&image_create_info, VMA_MEMORY_USAGE_AUTO, &imageAllocation);
    context->allocator->allocateBuffer(&staging_create_info, VMA_MEMORY_USAGE_CPU_ONLY, &stagingBuffer);

    void* mapped = stagingBuffer.mapMemory();

    memcpy(mapped, pixels, size);
    stagingBuffer.unmapMemory();

    loadTransferQueue();
    stbi_image_free(pixels);

    return imageAllocation;
}

VulkanAllocator::VulkanImageAllocation ImageTextureLoader::createImageFromBuffer(VkFormat format, uint32_t width,
        uint32_t height, void* data, size_t n) {

    this->width = width;
    this->height = height;

    VkBufferCreateInfo staging_create_info { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    staging_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    staging_create_info.size = n;
    staging_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VkImageCreateInfo image_create_info { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };

    image_create_info.extent = VkExtent3D {width, height, 1};
    image_create_info.format = format;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    context->allocator->allocateImage(&image_create_info, VMA_MEMORY_USAGE_AUTO, &imageAllocation);
    context->allocator->allocateBuffer(&staging_create_info, VMA_MEMORY_USAGE_CPU_ONLY, &stagingBuffer);

    void* mapped = stagingBuffer.mapMemory();

    memcpy(mapped, data, n);
    stagingBuffer.unmapMemory();

    loadTransferQueue();

    return imageAllocation;
}

ImageTextureLoader::ImageTextureLoader(VulkanContext *context)  {
    this->context = context;
}

void ImageTextureLoader::loadTransferQueue() {
    TransferQueue queue(*context);
    queue.begin();
    queue.applyBarrier(
            PipelineBarrierBuilder()
                    .forImage(imageAllocation.image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0 ,1))
                    .withInitialLayout(vk::ImageLayout::eUndefined)
                    .withFinalLayout(vk::ImageLayout::eTransferDstOptimal)
                    .waitFor(vk::PipelineStageFlagBits2::eTopOfPipe)
                    .beforeDoing(vk::PipelineStageFlagBits2::eTransfer)
                    .whichUses(vk::AccessFlagBits2::eTransferWrite)
                    .build()
            );
    VkBufferImageCopy region {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);

    region.imageOffset = {0,0,0};
    region.imageExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            1
    };

    queue.copyBufferToImage(stagingBuffer.buffer, imageAllocation.image, vk::ImageLayout::eTransferDstOptimal, region);

    queue.applyBarrier(
            PipelineBarrierBuilder()
                    .forImage(imageAllocation.image, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0 ,1))
                    .withInitialLayout(vk::ImageLayout::eTransferDstOptimal)
                    .withFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .waitFor(vk::PipelineStageFlagBits2::eTransfer)
                    .whichUses(vk::AccessFlagBits2::eTransferWrite)
                    .beforeDoing(vk::PipelineStageFlagBits2::eFragmentShader)
                    .whichUses(vk::AccessFlagBits2::eShaderRead)
                    .build()
        );

    queue.submit();
    stagingBuffer.destroy();
}
