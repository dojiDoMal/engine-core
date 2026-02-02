#include "renderer/backends/directx12/d3d12_renderer_backend.hpp"
#include "scene.hpp"
#include "stb_image_header.hpp"

#include "game_object_manager.hpp"
#include "scene_loader.hpp"
#include "window/window_desc.hpp"
#include "window/window_manager.hpp"
#include "logger.hpp"
#include "renderer/renderer_factory.hpp"

#ifndef PLATFORM_WEBGL
#include "renderer/backends/vulkan/vulkan_renderer_backend.hpp"
#endif

#include <cstdio>
#include <memory>

#include <SDL2/SDL.h>

#ifdef PLATFORM_WEBGL
GraphicsAPI graphicsAPI = GraphicsAPI::WEBGL;
#else
// Escolha a API aqui: GraphicsAPI::OPENGL ou GraphicsAPI::VULKAN
GraphicsAPI graphicsAPI = GraphicsAPI::DIRECTX12;
#endif

Scene scene;
GameObjectManager gameObjects;
std::unique_ptr<WindowManager> windowMan;
RendererBackend* rendererBackend = nullptr;
WindowDesc winDesc;

void init() {
    
    winDesc.title = "Engine";
    winDesc.width = 800;
    winDesc.height = 600;
    
    windowMan = std::make_unique<WindowManager>();
    windowMan->setGraphicsApi(graphicsAPI);
    windowMan->init(winDesc);

    rendererBackend = windowMan->getRenderer()->getRendererBackend();

    std::string sceneFilePath = "scene.scnb";
    scene.setCamera(SceneLoader::loadCamera(sceneFilePath, *rendererBackend));
    scene.setLights(SceneLoader::loadLights(sceneFilePath));
    scene.setGameObjects(SceneLoader::loadGameObjects(sceneFilePath, graphicsAPI, rendererBackend));
}

#ifdef PLATFORM_WEBGL
#include <emscripten.h>
#endif

#ifdef PLATFORM_WEBGL
void main_loop() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT ||
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            emscripten_cancel_main_loop();
            return;
        }
    }

    windowMan->getRenderer()->render(scene);
    SDL_GL_SwapWindow(windowMan->getWindow());
}
#else
void main_loop() {
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }
        
        windowMan->getRenderer()->render(scene);

        if (graphicsAPI == GraphicsAPI::VULKAN) {
            auto* vkBackend = dynamic_cast<VulkanRendererBackend*>(rendererBackend);
            if (vkBackend) vkBackend->present();
        } else if (graphicsAPI == GraphicsAPI::DIRECTX12) {
            auto* d3d12Backend = dynamic_cast<D3D12RendererBackend*>(rendererBackend);
            if (d3d12Backend) d3d12Backend->present();
        } else {
            SDL_GL_SwapWindow(windowMan->getWindow());
        }
    }

    SDL_Quit();
}

#endif

int main(int argc, char* argv[]) {
        Logger::init("engine");

        init();

#ifdef PLATFORM_WEBGL
        emscripten_set_main_loop(main_loop, 0, 1);
#else
        main_loop();
#endif

        Logger::shutdown();
        return 0;
}
