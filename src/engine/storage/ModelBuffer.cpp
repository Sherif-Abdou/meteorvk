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

void ModelBuffer::updateBuffer(const ModelBuffer::PerModelBuffer &permodel, unsigned int index) {
    buffer.updateBuffer(permodel, index);
}

void ModelBuffer::resetIndex() {
    current_index = 0;
}

void ModelBuffer::nextIndex() {
    current_index += 1;
}

unsigned long ModelBuffer::getOffset() {
    return buffer.getOffsetForIndex(current_index);
}

void ModelBuffer::attachOffsetToDescriptor(DescriptorSet &descriptorSet, unsigned int index) {
    descriptorSet.dynamic_offsets[index] = getOffset();
}

void ModelBuffer::writeBuffer(DescriptorSet &descriptorSet, unsigned int binding) {
    buffer.writeToDescriptor(descriptorSet, binding);
}

void ModelBuffer::destroy() {
    buffer.destroy();
}

