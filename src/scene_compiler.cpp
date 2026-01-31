#include "color.hpp"
#include "scene_format.hpp"
#include <array>
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

  scene.lightCount = 0;
  if (j.contains("lights")) {
    auto &lights = j["lights"];
    scene.lightCount = lights.size();
    for (size_t i = 0; i < lights.size() && i < 32; i++) {
      std::string type = lights[i]["type"];
      if (type == "DIRECTIONAL") scene.lights[i].type = 0;
      else if (type == "POINT") scene.lights[i].type = 1;
      else if (type == "SPOT") scene.lights[i].type = 2;
      else scene.lights[i].type = 0; // default

      std::array<float, 3> direction = lights[i]["direction"];
      for (int j = 0; j < 3; j++)
        scene.lights[i].direction[j] = direction[j];
    }
  }

  scene.meshCount = 0;
  if (j.contains("meshes")) {
    auto &meshes = j["meshes"];
    scene.meshCount = meshes.size();
    for (size_t i = 0; i < meshes.size() && i < 32; i++) {
      std::string objPath = meshes[i]["objPath"];
      std::string vertPath = meshes[i]["material"]["vertexShaderPath"];
      std::string fragPath = meshes[i]["material"]["fragmentShaderPath"];
      std::array<float, 4> color = meshes[i]["material"]["color"];
      std::strncpy(scene.meshes[i].objPath, objPath.c_str(), 255);
      std::strncpy(scene.meshes[i].material.vertexShaderPath, vertPath.c_str(), 255);
      std::strncpy(scene.meshes[i].material.fragmentShaderPath, fragPath.c_str(), 255);
      
      scene.meshes[i].material.color = {color[0], color[1], color[2], color[3]};
    }
  }

  std::ofstream output(argv[2], std::ios::binary);
  output.write(reinterpret_cast<char *>(&scene), sizeof(CompiledScene));

  return 0;
}
