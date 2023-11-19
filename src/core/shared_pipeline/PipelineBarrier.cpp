//
// Created by Sherif Abdou on 11/11/23.
//

#include "PipelineBarrier.h"

void PipelineBarrier::applyBarrier(vk::CommandBuffer buffer) {
    info.setMemoryBarriers(memory_barriers);
    info.setBufferMemoryBarriers(buffer_memory_barriers);
    info.setImageMemoryBarriers(image_memory_barriers);
    buffer.pipelineBarrier2(info);
}


