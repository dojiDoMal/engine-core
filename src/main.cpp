#include "stb_image_header.hpp"

#include "game_object_manager.hpp"
#include "scene_loader.hpp"
#include "window_manager.hpp"
#include "logger.hpp"

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

GameObjectManager gameObjects;
std::unique_ptr<WindowManager> windowMan;
Renderer* windowRenderer = nullptr;
RendererBackend* rendererBackend = nullptr;
SDL_Window* currentWindow = nullptr;
std::unique_ptr<Camera> mainCameraOwner;

void init() {
    windowMan = std::make_unique<WindowManager>(graphicsAPI);
    windowRenderer = windowMan->getRenderer();
    rendererBackend = windowRenderer->getRendererBackend();
    currentWindow = windowMan->getWindow();

    std::string sceneFilePath = "scene.scnb";

    mainCameraOwner = SceneLoader::loadCamera(sceneFilePath, *rendererBackend);
    rendererBackend->setCamera(mainCameraOwner.get());

    auto lights = SceneLoader::loadLights(sceneFilePath);
    rendererBackend->setLights(lights);

    auto objects = SceneLoader::loadMeshes(sceneFilePath, graphicsAPI, rendererBackend);
    for (auto& obj : objects) {
        gameObjects.add(obj.release());
    }
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

    windowRenderer->clearScreen();
    windowRenderer->render(gameObjects.get());
    SDL_GL_SwapWindow(currentWindow);
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

        windowRenderer->clearScreen();

        windowRenderer->render(gameObjects.get());

        if (graphicsAPI == GraphicsAPI::VULKAN) {
            auto* vkBackend = dynamic_cast<VulkanRendererBackend*>(rendererBackend);
            if (vkBackend) vkBackend->present();
        } else {
            SDL_GL_SwapWindow(currentWindow);
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
