//
// Created by Sherif Abdou on 9/26/23.
//

#include "ModelBuffer.h"

unsigned int ModelBuffer::getSize() const {
    return size;
}

ModelBuffer::ModelBuffer(VulkanContext &context, unsigned int size) : context(context), size(size), buffer(DynamicUniformBuffer<PerModelBuffer>(context)) {
    buffer.allocateBuffer(size);
}

void ModelBuffer::updateBuffer(ModelBuffer::PerModelBuffer &permodel, unsigned int index) {
}
