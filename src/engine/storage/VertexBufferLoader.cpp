#include "VertexBufferLoader.h"
#include "core/storage/NewOBJFile.h"
#include "core/storage/VertexBuffer.h"


VertexBufferLoader::VertexBufferLoader(VulkanContext* context, ModelBuffer* model_buffer):
    context(context), model_buffer(model_buffer) {

}

void VertexBufferLoader::addModel(const std::string& name, const std::string& model_path, glm::mat4 model_matrix) {
    uint32_t index = size;
    name_index_map[name] = index;
    vertex_buffers.push_back(nullptr);
    size++;
    should_update = true;

    auto handle = std::async(std::launch::async, [index, model_path, model_matrix, this]() {
            auto vertex_buffer = std::make_unique<VertexBuffer>(VertexBufferLoader::createVertexBuffer(this->context, model_path.c_str()));
            vertex_buffer->init();

            this->data_mutex.lock();
            model_buffer->updateBuffer({
                    model_matrix,
                    Material(),
                    }, index);
            vertex_buffers[index] = std::move(vertex_buffer);
            this->data_mutex.unlock();
            });
    handles.push_back(std::move(handle));
}

void VertexBufferLoader::updateModelMatrix(const std::string& name, glm::mat4 model_matrix) {
    model_buffer->updateBuffer({
            model_matrix,
            Material(),
            }, name_index_map[name]);
}

void VertexBufferLoader::waitForAll() {
    for (auto& handle: handles) {
        handle.wait();
    }
    handles.clear();
}

VertexBuffer VertexBufferLoader::createVertexBuffer(VulkanContext *context, const char *path) {
    VertexBuffer buffer(context, true);
    NewOBJFile file = NewOBJFile::fromFilePath(path);
    auto raw = file.exportIndexedBuffer();

    buffer.vertices = std::move(raw.vertices);
    buffer.indices = std::move(raw.indices);

    /*
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
       */
    return buffer;
}

void VertexBufferLoader::attachToCommandBuffer(std::vector<VertexBuffer*> *buffer) {
    if (!should_update) {
        return;
    }
    buffer->clear();
    for (auto& vbo: vertex_buffers) {
        buffer->push_back(vbo.get());
    }
    should_update = false;
}