#include "vulkan_shader_program.hpp"
#include "renderer/backends/vulkan/vulkan_renderer_backend.hpp"
#include "shader_asset.hpp"

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

void* VulkanShaderProgram::getHandle() const {
    return (void*)pipeline;
}

bool VulkanShaderProgram::isValid() const {
    return true; // Vulkan usa pipeline global, não por material
}
