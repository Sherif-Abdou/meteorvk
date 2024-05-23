#ifndef VULKAN_ENGINE_VERTEXBUFFERLOADER_H
#define VULKAN_ENGINE_VERTEXBUFFERLOADER_H

#include "core/VulkanContext.h"
#include "core/storage/VertexBuffer.h"
#include "core_v2/NewDescriptorManager.h"
#include "engine/storage/ModelBuffer.h"

#include <mutex>
#include <future>

class VertexBufferLoader {
    private:
        VulkanContext* context = nullptr;
        ModelBuffer* model_buffer = nullptr;
        NewDescriptorManager* descriptor_manager = nullptr;
        std::unordered_map<std::string, uint32_t> name_index_map {};
        std::mutex data_mutex {};
        std::vector<std::unique_ptr<VertexBuffer>> vertex_buffers {};

        uint32_t size = 0;

        std::vector<std::future<void>> handles {};

        static VertexBuffer createVertexBuffer(VulkanContext *context, const char *path);

        bool should_update = false;
    public:
        VertexBufferLoader(VulkanContext* context, ModelBuffer* model_buffer, NewDescriptorManager* descriptor_manager = nullptr);

        void addModel(const std::string& name, const std::string& model_path, glm::mat4 model_matrix, bool useTexture=false);

        void updateModelMatrix(const std::string& name, glm::mat4 model_matrix);

        void attachToCommandBuffer(std::vector<VertexBuffer*> *buffer);

        DescriptorSet* getDescriptorForModel(const std::string& name);

        void waitForAll();
};

#endif
