#include "vulkan_shader_program.hpp"
#include "renderer/backends/vulkan/vulkan_renderer_backend.hpp"
#include "shader_asset.hpp"
#include <cstring>

VulkanShaderProgram::~VulkanShaderProgram() {
    if (pipeline) {
        vkDestroyPipeline(backend->getDevice(), pipeline, nullptr);
    }
    if (pipelineLayout) {
        vkDestroyPipelineLayout(backend->getDevice(), pipelineLayout, nullptr);
    }
}

bool VulkanShaderProgram::attachShader(const ShaderAsset& shader) {
    VkShaderModule module = *static_cast<VkShaderModule*>(shader.getHandle());
    shaderModules.push_back(module);
    return true;
}

bool VulkanShaderProgram::link() {
    // Em Vulkan, "link" significa criar o pipeline
    // Isso requer render pass, que deve vir do backend
    // Por enquanto, apenas retorna true
    return true;
}

void VulkanShaderProgram::use() {
    // Em Vulkan, "use" é feito via vkCmdBindPipeline no command buffer
}

void VulkanShaderProgram::setUniformBuffer(const char* name, int binding, const void* data, size_t size) {
    if (binding == 1 && backend) {
        void* mapped;
        vkMapMemory(backend->getDevice(), backend->getMaterialBufferMemory(), 0, size, 0, &mapped);
        memcpy(mapped, data, size);
        vkUnmapMemory(backend->getDevice(), backend->getMaterialBufferMemory());
    }
}

void* VulkanShaderProgram::getHandle() const {
    return (void*)pipeline;
}

bool VulkanShaderProgram::isValid() const {
    return true; // Vulkan usa pipeline global, não por material
}
