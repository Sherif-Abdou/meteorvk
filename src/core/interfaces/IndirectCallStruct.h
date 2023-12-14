//
// Created by Sherif Abdou on 12/13/23.
//

#ifndef VULKAN_ENGINE_INDIRECTCALLSTRUCT_H
#define VULKAN_ENGINE_INDIRECTCALLSTRUCT_H


#include <cstdint>

struct IndirectCallStruct {
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    uint32_t firstInstance;
};


#endif //VULKAN_ENGINE_INDIRECTCALLSTRUCT_H
