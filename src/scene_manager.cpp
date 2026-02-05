#include "scene_manager.hpp"
#include "renderer/renderer_backend.hpp"
#include "scene_loader.hpp"

SceneManager::~SceneManager() {
    if (activeScene != nullptr) {
        delete activeScene;
    }
}

void SceneManager::addScene(const std::string& name, const std::string& path) {
    sceneRegistry[name] = path;
}

Scene* SceneManager::getActiveScene() const { return activeScene; }

// TODO: revisar lógica
void SceneManager::loadScene(const std::string& name) {
    
    // nao tenho certeza de precisa ou se deve fazer isso
    if (activeScene != nullptr) {
        delete activeScene;
    }

    activeSceneName = name;
    auto path = sceneRegistry[name];

    activeScene = new Scene();
    activeScene->setCamera(sceneLoader.loadCamera(path));
    activeScene->setLights(sceneLoader.loadLights(path));
    activeScene->setGameObjects(sceneLoader.loadGameObjects(path));
}

void SceneManager::setRendererBackend(RendererBackend& rendererBackend) {
    sceneLoader.setRendererBackend(rendererBackend);
}