#ifndef SCENE_LOADER_HPP
#define SCENE_LOADER_HPP

#include "camera.hpp"
#include "game_object.hpp"
#include "mesh.hpp"
#include "renderer/renderer_backend.hpp"
#include <memory>
#include <string>
#include <vector>
#include "light.hpp"

class SceneLoader {
private:
  RendererBackend* rendererBackend = nullptr;
  std::unique_ptr<Mesh> loadObjMesh(const std::string& filepath, bool shadeSmooth);

public:
  SceneLoader();
  void setRendererBackend(RendererBackend&);
  Camera* loadCamera(const std::string& filepath);
  std::vector<GameObject*>* loadGameObjects(const std::string& filepath);
  std::vector<Light>* loadLights(const std::string& filepath);

};

#endif
