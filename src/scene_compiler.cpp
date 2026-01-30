#include "scene_format.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char *argv[]) {
  std::ifstream input(argv[1]);
  json j = json::parse(input);

  CompiledScene scene;
  auto &cam = j["camera"];

  for (int i = 0; i < 4; i++)
    scene.camera.background_color[i] = cam["background_color"][i];
  scene.camera.fov = cam["fov"];
  for (int i = 0; i < 2; i++)
    scene.camera.view_rect[i] = cam["view_rect"][i];
  for (int i = 0; i < 3; i++)
    scene.camera.position[i] = cam["position"][i];

  if (cam.contains("skybox")) {
    scene.camera.hasSkybox = true;

    auto &skybox = cam["skybox"];

    std::string vertPath = skybox["material"]["vertexShaderPath"];
    std::string fragPath = skybox["material"]["fragmentShaderPath"];

    std::strncpy(scene.camera.skybox.material.vertexShaderPath, vertPath.c_str(), 255);
    std::strncpy(scene.camera.skybox.material.fragmentShaderPath, fragPath.c_str(), 255);

    for (int i = 0; i < 6; i++) {
      std::string texPath = skybox["cubeMapTextures"][i];
      std::strncpy(scene.camera.skybox.cubeMapTextures[i], texPath.c_str(),
                   255);
    }
  } else {
    scene.camera.hasSkybox = false;
  }

  scene.meshCount = 0;
  if (j.contains("meshes")) {
    auto &meshes = j["meshes"];
    scene.meshCount = meshes.size();
    for (size_t i = 0; i < meshes.size() && i < 32; i++) {
      std::string objPath = meshes[i]["objPath"];
      std::string vertPath = meshes[i]["material"]["vertexShaderPath"];
      std::string fragPath = meshes[i]["material"]["fragmentShaderPath"];
      std::strncpy(scene.meshes[i].objPath, objPath.c_str(), 255);
      std::strncpy(scene.meshes[i].material.vertexShaderPath, vertPath.c_str(), 255);
      std::strncpy(scene.meshes[i].material.fragmentShaderPath, fragPath.c_str(), 255);
    }
  }

  std::ofstream output(argv[2], std::ios::binary);
  output.write(reinterpret_cast<char *>(&scene), sizeof(CompiledScene));

  return 0;
}
