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
  static std::string getShaderPath(const std::string& basePath, GraphicsAPI api);
  static std::unique_ptr<Mesh> loadObjMesh(const std::string& filepath, GraphicsAPI api);

public:
  static std::unique_ptr<Camera> loadCamera(const std::string& filepath,
                                            RendererBackend& rendererBackend);
  static std::vector<std::unique_ptr<GameObject>> loadMeshes(const std::string& filepath, GraphicsAPI api, RendererBackend* backend);
  static std::vector<Light> loadLights(const std::string& filepath);

};

#endif
