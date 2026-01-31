#include "shader_compiler_factory.hpp"

#ifdef PLATFORM_WEBGL
#include "renderer/backends/webgl/web_gl_shader_compiler.hpp"
#else
#include "renderer/backends/opengl/open_gl_shader_compiler.hpp"
#include "renderer/backends/vulkan/vulkan_shader_compiler.hpp"
#endif

std::unique_ptr<ShaderCompiler> ShaderCompilerFactory::create(GraphicsAPI api, void* context) {
    switch(api) {         
        #ifdef PLATFORM_WEBGL
        case GraphicsAPI::WEBGL:
            return std::make_unique<WebGLShaderCompiler>(); 
        #else
        case GraphicsAPI::OPENGL:
            return std::make_unique<OpenGLShaderCompiler>();
        case GraphicsAPI::VULKAN:
            return std::make_unique<VulkanShaderCompiler>(static_cast<VulkanRendererBackend*>(context));
        #endif
        default:
            return nullptr;
    }
}
