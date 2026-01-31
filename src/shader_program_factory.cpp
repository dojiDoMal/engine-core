#include "shader_program_factory.hpp"

#ifdef PLATFORM_WEBGL
#include "renderer/backends/webgl/web_gl_shader_program.hpp"
#else
#include "renderer/backends/opengl/open_gl_shader_program.hpp"
#include "renderer/backends/vulkan/vulkan_shader_program.hpp"
#endif

std::unique_ptr<ShaderProgram> ShaderProgramFactory::create(GraphicsAPI api, void* context) {
    switch(api) {        
        #ifdef PLATFORM_WEBGL  
        case GraphicsAPI::WEBGL:
            return std::make_unique<WebGLShaderProgram>(); 
        #else
        case GraphicsAPI::OPENGL:
            return std::make_unique<OpenGLShaderProgram>();
        case GraphicsAPI::VULKAN:
            return std::make_unique<VulkanShaderProgram>(static_cast<VulkanRendererBackend*>(context));
        #endif   
        default:
            return nullptr;
    }
}
