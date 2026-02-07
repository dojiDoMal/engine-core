#include "color.hpp"
#include "scene_format.hpp"
#include "vector3.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    std::ifstream input(argv[1]);
    json j = json::parse(input);

    CompiledScene scene;
    auto& cam = j["camera"];

    for (int i = 0; i < 4; i++)
        scene.camera.background_color[i] = cam["background_color"][i];
    scene.camera.fov = cam["fov"];
    for (int i = 0; i < 2; i++)
        scene.camera.view_rect[i] = cam["view_rect"][i];
    for (int i = 0; i < 3; i++)
        scene.camera.position[i] = cam["position"][i];

    if (cam.contains("skybox")) {
        scene.camera.hasSkybox = true;

        auto& skybox = cam["skybox"];

        std::string vertPath = skybox["material"]["vertexShaderPath"];
        std::string fragPath = skybox["material"]["fragmentShaderPath"];

        std::snprintf(scene.camera.skybox.material.vertexShaderPath,
                      sizeof(scene.camera.skybox.material.vertexShaderPath), "%s",
                      vertPath.c_str());

        std::snprintf(scene.camera.skybox.material.fragmentShaderPath,
                      sizeof(scene.camera.skybox.material.fragmentShaderPath), "%s",
                      fragPath.c_str());

        for (int i = 0; i < 6; i++) {
            std::string texPath = skybox["cubeMapTextures"][i];
            std::snprintf(scene.camera.skybox.cubeMapTextures[i],
                          sizeof(scene.camera.skybox.cubeMapTextures[i]), "%s", texPath.c_str());
        }
    } else {
        scene.camera.hasSkybox = false;
    }

    scene.lightCount = 0;
    if (j.contains("lights")) {
        auto& lights = j["lights"];
        scene.lightCount = lights.size();
        for (size_t i = 0; i < lights.size() && i < 32; i++) {
            std::string type = lights[i]["type"];
            if (type == "DIRECTIONAL")
                scene.lights[i].type = 0;
            else if (type == "POINT")
                scene.lights[i].type = 1;
            else if (type == "SPOT")
                scene.lights[i].type = 2;
            else
                scene.lights[i].type = 0; // default

            Vector3 direction;
            direction.x = lights[i]["direction"][0];
            direction.y = lights[i]["direction"][1];
            direction.z = lights[i]["direction"][2];
            scene.lights[i].direction = direction;
        }
    }

    scene.gameObjectCount = 0;
    if (j.contains("gameObjects")) {
        auto& gameObjects = j["gameObjects"];
        scene.gameObjectCount = gameObjects.size();
        for (size_t i = 0; i < gameObjects.size() && i < 32; i++) {
            auto& go = gameObjects[i];
            auto& goData = scene.gameObjects[i];

            goData.componentCount = 0;

            if (go.contains("components")) {
                auto& components = go["components"];
                goData.componentCount = components.size();

                for (size_t j = 0; j < components.size() && j < 8; j++) {
                    auto& comp = components[j];
                    std::string type = comp["type"];

                    if (type == "MESH_RENDERER") {
                        goData.components[j].type = ComponentType::MESH_RENDERER;

                        std::string objPath = comp["objPath"];
                        std::string vertPath = comp["material"]["vertexShaderPath"];
                        std::string fragPath = comp["material"]["fragmentShaderPath"];
                        std::array<float, 4> color = comp["material"]["color"];

                        bool shadeSmooth = comp.value("shadeSmooth", true);
                        goData.components[j].meshRenderer.shadeSmooth = shadeSmooth;

                        std::snprintf(goData.components[j].meshRenderer.objPath,
                                      sizeof(goData.components[j].meshRenderer.objPath), "%s",
                                      objPath.c_str());

                        std::snprintf(
                            goData.components[j].meshRenderer.material.vertexShaderPath,
                            sizeof(goData.components[j].meshRenderer.material.vertexShaderPath),
                            "%s", vertPath.c_str());

                        std::snprintf(
                            goData.components[j].meshRenderer.material.fragmentShaderPath,
                            sizeof(goData.components[j].meshRenderer.material.fragmentShaderPath),
                            "%s", fragPath.c_str());

                        goData.components[j].meshRenderer.material.color = {color[0], color[1],
                                                                            color[2], color[3]};
                    } else if (type == "SPRITE_RENDERER") {
                        goData.components[j].type = ComponentType::SPRITE_RENDERER;

                        std::string texPath = comp["texturePath"];
                        std::string vertPath = comp["material"]["vertexShaderPath"];
                        std::string fragPath = comp["material"]["fragmentShaderPath"];
                        std::array<float, 4> color = comp["material"]["color"];
                        float width = comp.value("width", 1.0f);
                        float height = comp.value("height", 1.0f);

                        std::snprintf(goData.components[j].spriteRenderer.texturePath,
                                      sizeof(goData.components[j].spriteRenderer.texturePath), "%s",
                                      texPath.c_str());

                        std::snprintf(
                            goData.components[j].spriteRenderer.material.vertexShaderPath,
                            sizeof(goData.components[j].spriteRenderer.material.vertexShaderPath),
                            "%s", vertPath.c_str());

                        std::snprintf(
                            goData.components[j].spriteRenderer.material.fragmentShaderPath,
                            sizeof(goData.components[j].spriteRenderer.material.fragmentShaderPath),
                            "%s", fragPath.c_str());

                        goData.components[j].spriteRenderer.material.color = {color[0], color[1],
                                                                              color[2], color[3]};
                        goData.components[j].spriteRenderer.width = width;
                        goData.components[j].spriteRenderer.height = height;
                    }
                }
            }
        }
    }

    std::ofstream output(argv[2], std::ios::binary);
    output.write(reinterpret_cast<char*>(&scene), sizeof(CompiledScene));

    return 0;
}
