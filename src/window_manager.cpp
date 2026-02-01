#include <stdexcept>
#define CLASS_NAME "WindowManager"
#include "log_macros.hpp"
#include "renderer/renderer_backend.hpp"
#ifdef PLATFORM_WEBGL
#include "renderer/backends/webgl/web_gl_renderer_backend.hpp"
#else
#include "renderer/backends/opengl/open_gl_renderer_backend.hpp"
#include "renderer/backends/vulkan/vulkan_renderer_backend.hpp"
#include "renderer/backends/directx12/d3d12_renderer_backend.hpp"
#endif

#include "window_manager.hpp"
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL_syswm.h>

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
    SDL_Window* win = nullptr;

    if (api == GraphicsAPI::OPENGL || api == GraphicsAPI::WEBGL) {

        #ifdef PLATFORM_WEBGL
            renderer->setRendererBackend(new WebGLRendererBackend());
        #else
            renderer->setRendererBackend(new OpenGLRendererBackend());
        #endif

        renderer->initContext();
        
        win = SDL_CreateWindow(
            this->title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            this->width,
            this->height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
        
        if(!win) {
            SDL_Quit();
            return false;
        }
        
        glContext = SDL_GL_CreateContext(win);
        if(!glContext) {
            SDL_DestroyWindow(win);
            SDL_Quit();
            return false;
        }
    } 
    #ifndef PLATFORM_WEBGL
    else if (api == GraphicsAPI::VULKAN) {
        LOG_INFO("Creating vulkan backend!");
        auto* vkBackend = new VulkanRendererBackend();
        renderer->setRendererBackend(vkBackend);
        
        // Criar janela PRIMEIRO para SDL poder fornecer extensões
        win = SDL_CreateWindow(
            this->title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            this->width,
            this->height,
            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
        );
        
        if(!win) {
            LOG_ERROR(std::string("Failed to create window: ") + SDL_GetError());
            delete vkBackend;
            delete renderer;
            SDL_Quit();
            return false;
        }

        vkBackend->setWindow(win);
        
        // Agora criar instância Vulkan (precisa da janela para extensões)
        LOG_INFO("Initializing renderer context!");
        renderer->initContext();
        
        // Criar surface Vulkan após ter a janela e instância
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(win, vkBackend->getInstance(), &surface)) {
            LOG_ERROR("Failed to create vulkan surface");
            SDL_DestroyWindow(win);
            delete vkBackend;
            delete renderer;
            SDL_Quit();
            return false;
        }
        vkBackend->setSurface(surface);
    }
    #endif
    else if (api == GraphicsAPI::DIRECTX12) {
        auto* d3d12Backend = new D3D12RendererBackend();
        renderer->setRendererBackend(d3d12Backend);
        
        win = SDL_CreateWindow(
            this->title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            this->width,
            this->height,
            SDL_WINDOW_SHOWN
        );
        
        if(!win) {
            delete d3d12Backend;
            delete renderer;
            SDL_Quit();
            return false;
        }
        
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(win, &wmInfo);
        d3d12Backend->setHwnd(wmInfo.info.win.window);
    }

    this->setWindow(win);
    LOG_INFO("Initializing renderer!");
    if (!renderer->init()) {
        LOG_ERROR("Failed to init renderer");
        return false;
    }

    return true;
}
