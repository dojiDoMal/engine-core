#include "scene.hpp"

Scene::~Scene() {
    if (gameObjects) {
        for (GameObject* obj : *gameObjects) {
            delete obj;
        }
        delete gameObjects;
    }

    if (lights != nullptr) {
        delete lights;
    }

    if (mainCamera != nullptr) {
        delete mainCamera;
    }
}