#ifdef _WIN32
#include "renderer/backends/directx12/d3d12_renderer_backend.hpp"
#endif

#include "scene.hpp"
#include "scene_manager.hpp"
#include "stb_image_header.hpp"

#include "game_object_manager.hpp"
#include "window/window_desc.hpp"
#include "window/window_manager.hpp"
#include "logger.hpp"
#include <SDL2/SDL_keycode.h>

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
GraphicsAPI graphicsAPI = GraphicsAPI::OPENGL;
#endif

Scene scene;
GameObjectManager gameObjects;
std::unique_ptr<WindowManager> screenManager;
std::unique_ptr<SceneManager> sceneManager;
RendererBackend* rendererBackend = nullptr;
WindowDesc winDesc;

void init() {
    
    winDesc.title = "Engine";
    winDesc.width = 800;
    winDesc.height = 600;
    
    screenManager = std::make_unique<WindowManager>();
    screenManager->setGraphicsApi(graphicsAPI);
    screenManager->init(winDesc);

    rendererBackend = screenManager->getRenderer()->getRendererBackend();

    sceneManager = std::make_unique<SceneManager>();
    sceneManager->setRendererBackend(*rendererBackend);
    sceneManager->addScene("cena1", "scene.scnb");
    sceneManager->addScene("cena2", "new_scene.scnb");
    sceneManager->loadScene("cena1");
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

    screenManager->getRenderer()->render(scene);
    SDL_GL_SwapWindow(screenManager->getWindow());
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
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                sceneManager->loadScene("cena2");
            }
        }
        
        screenManager->render(*sceneManager->getActiveScene());

        if (graphicsAPI == GraphicsAPI::VULKAN) {
            auto* vkBackend = dynamic_cast<VulkanRendererBackend*>(rendererBackend);
            if (vkBackend) vkBackend->present();
        #ifdef _WIN32
        } else if (graphicsAPI == GraphicsAPI::DIRECTX12) {
            auto* d3d12Backend = dynamic_cast<D3D12RendererBackend*>(rendererBackend);
            if (d3d12Backend) d3d12Backend->present();
        #endif
        } else {
            SDL_GL_SwapWindow(screenManager->getWindow());
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
