#ifndef SCENE_FORMAT_HPP
#define SCENE_FORMAT_HPP

#include <cstdint>
#include "color.hpp"

struct MaterialData {
  char vertexShaderPath[256];
  char fragmentShaderPath[256];
  ColorRGBA color;
};

struct SkyboxData {
  char cubeMapTextures[6][256];
  MaterialData material;
};

struct MeshData {
  char objPath[256];
  MaterialData material;
};

struct SceneCameraData {
  float background_color[4];
  float fov;
  float view_rect[2];
  double position[3];
  bool hasSkybox;
  SkyboxData skybox;
};

struct CompiledScene {
  uint32_t magic = 0x53434E45;
  SceneCameraData camera;
  uint32_t meshCount;
  MeshData meshes[32];
};

#endif