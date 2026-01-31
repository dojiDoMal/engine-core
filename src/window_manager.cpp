#define CLASS_NAME "WindowManager"
#ifdef PLATFORM_WEBGL
#include "renderer/backends/webgl/web_gl_renderer_backend.hpp"
#else
#include "renderer/backends/opengl/open_gl_renderer_backend.hpp"
#include "renderer/backends/vulkan/vulkan_renderer_backend.hpp"
#endif

#include "window_manager.hpp"
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_syswm.h>
#include "log_macros.hpp"

WindowManager::WindowManager(GraphicsAPI api) {
    init(api);
}

WindowManager::~WindowManager() {
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool WindowManager::init(GraphicsAPI api){
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        return false;
    }

    renderer = new Renderer();
    SDL_Window* window = nullptr;

    if (api == GraphicsAPI::OPENGL || api == GraphicsAPI::WEBGL) {

        #ifdef PLATFORM_WEBGL
            renderer->setRendererBackend(new WebGLRendererBackend());
        #else
            renderer->setRendererBackend(new OpenGLRendererBackend());
        #endif

        renderer->initContext();
        
        window = SDL_CreateWindow(
            this->title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            this->width,
            this->height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
        
        if(!window) {
            SDL_Quit();
            return false;
        }
        
        glContext = SDL_GL_CreateContext(window);
        if(!glContext) {
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
    } 
    #ifndef PLATFORM_WEBGL
    else if (api == GraphicsAPI::VULKAN) {
        auto* vkBackend = new VulkanRendererBackend();
        renderer->setRendererBackend(vkBackend);
        
        // Criar janela PRIMEIRO para SDL poder fornecer extensões
        window = SDL_CreateWindow(
            this->title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            this->width,
            this->height,
            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
        );
        
        if(!window) {
            SDL_Quit();
            return false;
        }
        
        // Agora criar instância Vulkan (precisa da janela para extensões)
        renderer->initContext();
        
        // Criar surface Vulkan após ter a janela e instância
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(window, vkBackend->getInstance(), &surface)) {
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }
        vkBackend->setSurface(surface);
    }
    #endif

    this->setWindow(window);
    //this->renderer = renderer;
    renderer->init();

    return true;
}
