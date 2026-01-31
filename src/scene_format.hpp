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

struct LightData {
    uint8_t type;  // 0=DIRECTIONAL, 1=POINT, 2=SPOT
    float direction[3];
    //float position[3];
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
  uint32_t lightCount;
  LightData lights[32];
};

#endif