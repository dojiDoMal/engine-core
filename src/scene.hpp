#ifndef SCENE_HPP
#define SCENE_HPP

#include "camera.hpp"
#include "game_object.hpp"
#include "light.hpp"

class Scene {
  private:
    Camera* mainCamera = nullptr;
    std::vector<GameObject*>* gameObjects = nullptr;
    std::vector<Light>* lights = nullptr;

  public:
    ~Scene();
    void setCamera(Camera* cam) { mainCamera = cam; };
    Camera* getCamera() const { return mainCamera; };

    void setGameObjects(std::vector<GameObject*>* gos) { gameObjects = gos; };
    std::vector<GameObject*>* getGameObjects() { return gameObjects; };
    const std::vector<GameObject*>* getGameObjects() const { return gameObjects; };

    void setLights(std::vector<Light>* l) { lights = l; };
    std::vector<Light>* getLights() { return lights; };
    const std::vector<Light>* getLights() const { return lights; };
};

#endif