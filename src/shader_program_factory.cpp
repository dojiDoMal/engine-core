#include "shader_program_factory.hpp"

#ifdef PLATFORM_WEBGL
#include "web_gl_shader_program.hpp"
#else
#include "open_gl_shader_program.hpp"
#include "vulkan_shader_program.hpp"
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
