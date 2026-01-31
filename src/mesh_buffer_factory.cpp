#include "mesh_buffer_factory.hpp"

#ifdef PLATFORM_WEBGL
#include "renderer/backends/webgl/web_gl_mesh_buffer.hpp"
#else
#include "renderer/backends/opengl/open_gl_mesh_buffer.hpp"
#include "renderer/backends/vulkan/vulkan_mesh_buffer.hpp"
#endif

std::unique_ptr<MeshBuffer> MeshBufferFactory::create(GraphicsAPI api, void* context) {
    switch(api) {        
        #ifdef PLATFORM_WEBGL
        case GraphicsAPI::WEBGL:
            return std::make_unique<WebGLMeshBuffer>();
        #else 
        case GraphicsAPI::OPENGL:
            return std::make_unique<OpenGLMeshBuffer>();
        case GraphicsAPI::VULKAN:
            return std::make_unique<VulkanMeshBuffer>(static_cast<VulkanRendererBackend*>(context));
        #endif
        default:
            return nullptr;
    }
}
