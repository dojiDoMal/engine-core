#define CLASS_NAME "SceneLoader"
#include "log_macros.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

#include <fstream>
#include "renderer/renderer_backend.hpp"
#include "material.hpp"
#include "mesh_renderer.hpp"
#include "scene_format.hpp"
#include "scene_loader.hpp"
#include "shader_asset.hpp"
#include "skybox.hpp"

SceneLoader::SceneLoader() : rendererBackend(nullptr) {}

void SceneLoader::setRendererBackend(RendererBackend& backend) {
    rendererBackend = &backend;
}

std::unique_ptr<Mesh> SceneLoader::loadObjMesh(const std::string& filepath, bool shadeSmooth) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str())) {
        LOG_ERROR("Unable to load obj: " + filepath);
        return nullptr;
    }

    std::vector<float> vertices;
    std::vector<float> normals;

    if (shadeSmooth && !attrib.normals.empty()) {
        // Usar normais do arquivo (smooth)
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

                if (index.normal_index >= 0) {
                    normals.push_back(attrib.normals[3 * index.normal_index + 0]);
                    normals.push_back(attrib.normals[3 * index.normal_index + 1]);
                    normals.push_back(attrib.normals[3 * index.normal_index + 2]);
                }
            }
        }
    } else {
        // Calcular normais flat (por face)
        for (const auto& shape : shapes) {
            for (size_t f = 0; f < shape.mesh.indices.size(); f += 3) {
                // Pegar 3 vértices do triângulo
                auto& i0 = shape.mesh.indices[f + 0];
                auto& i1 = shape.mesh.indices[f + 1];
                auto& i2 = shape.mesh.indices[f + 2];

                float v0[3] = {attrib.vertices[3 * i0.vertex_index + 0],
                               attrib.vertices[3 * i0.vertex_index + 1],
                               attrib.vertices[3 * i0.vertex_index + 2]};
                float v1[3] = {attrib.vertices[3 * i1.vertex_index + 0],
                               attrib.vertices[3 * i1.vertex_index + 1],
                               attrib.vertices[3 * i1.vertex_index + 2]};
                float v2[3] = {attrib.vertices[3 * i2.vertex_index + 0],
                               attrib.vertices[3 * i2.vertex_index + 1],
                               attrib.vertices[3 * i2.vertex_index + 2]};

                // Calcular normal da face
                float edge1[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2]};
                float edge2[3] = {v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2]};
                float normal[3] = {edge1[1] * edge2[2] - edge1[2] * edge2[1],
                                   edge1[2] * edge2[0] - edge1[0] * edge2[2],
                                   edge1[0] * edge2[1] - edge1[1] * edge2[0]};

                // Normalizar
                float len =
                    sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
                if (len > 0) {
                    normal[0] /= len;
                    normal[1] /= len;
                    normal[2] /= len;
                }

                // Adicionar vértices e mesma normal para os 3 vértices
                for (int i = 0; i < 3; i++) {
                    auto& idx = shape.mesh.indices[f + i];
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
                    vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
                    normals.push_back(normal[0]);
                    normals.push_back(normal[1]);
                    normals.push_back(normal[2]);
                }
            }
        }
    }

    auto mesh = std::make_unique<Mesh>();
    mesh->setVertices(vertices);
    mesh->setNormals(normals);
    return mesh;
}

Camera* SceneLoader::loadCamera(const std::string& filepath) {
    if (!validateSceneFile(filepath)) return nullptr;

    std::ifstream file(filepath, std::ios::binary);
    CompiledScene scene;
    file.read(reinterpret_cast<char*>(&scene), sizeof(CompiledScene));
    
    auto camera = new Camera();
    auto& cam = scene.camera;

    camera->setBackgroundColor({cam.background_color[0], cam.background_color[1],
                                cam.background_color[2], cam.background_color[3]});
    camera->setFov(cam.fov);
    camera->setViewRect(cam.view_rect[0], cam.view_rect[1]);
    camera->setPosition({(float)cam.position[0], (float)cam.position[1], (float)cam.position[2]});

    if (cam.hasSkybox) {
        auto skybox = std::make_unique<Skybox>();

        auto shaderExt = rendererBackend->getShaderExtension();
        auto skyboxVertexShaderPtr = std::make_unique<ShaderAsset>(
            cam.skybox.material.vertexShaderPath + shaderExt, ShaderType::VERTEX);
        skyboxVertexShaderPtr->setShaderCompiler(rendererBackend->createShaderCompiler());

        auto skyboxFragmentShaderPtr = std::make_unique<ShaderAsset>(
            cam.skybox.material.fragmentShaderPath + shaderExt, ShaderType::FRAGMENT);
        skyboxFragmentShaderPtr->setShaderCompiler(rendererBackend->createShaderCompiler());

        auto skyboxMaterial = std::make_unique<Material>();
        skyboxMaterial->setShaderProgram(rendererBackend->createShaderProgram());
        skyboxMaterial->setVertexShader(std::move(skyboxVertexShaderPtr));
        skyboxMaterial->setFragmentShader(std::move(skyboxFragmentShaderPtr));
        skyboxMaterial->init();

        std::vector<std::string> faces;
        for (const auto row : cam.skybox.cubeMapTextures) {
            faces.push_back(row);
        }

        unsigned int cubemapID = rendererBackend->createCubemapTexture(faces);
        skybox->setTextureID(cubemapID);
        skybox->setMaterial(std::move(skyboxMaterial));
        skybox->init();
        camera->setSkybox(std::move(skybox));
    }

    return camera;
}

std::vector<GameObject*>* SceneLoader::loadGameObjects(const std::string& filepath) {
    if (!validateSceneFile(filepath)) return nullptr;

    std::ifstream file(filepath, std::ios::binary);
    CompiledScene scene;
    file.read(reinterpret_cast<char*>(&scene), sizeof(CompiledScene));
    
    auto objects = new std::vector<GameObject*>();

    for (uint32_t i = 0; i < scene.gameObjectCount; i++) {
        auto& goData = scene.gameObjects[i];
        auto gameObject = new GameObject();

        for (uint8_t j = 0; j < goData.componentCount; j++) {
            auto& comp = goData.components[j];

            if (comp.type == ComponentType::MESH_RENDERER) {
                auto& meshData = comp.meshRenderer;

                auto mesh = loadObjMesh(meshData.objPath, meshData.shadeSmooth);
                if (!mesh) {
                    LOG_ERROR("Failed to load mesh: " + std::string(meshData.objPath));
                    continue;
                }
                mesh->setMeshBuffer(rendererBackend->createMeshBuffer());
                mesh->configure();

                auto shaderExt = rendererBackend->getShaderExtension();
                auto vertexShader = std::make_unique<ShaderAsset>(
                    meshData.material.vertexShaderPath + shaderExt, ShaderType::VERTEX);
                vertexShader->setShaderCompiler(rendererBackend->createShaderCompiler());

                auto fragmentShader = std::make_unique<ShaderAsset>(
                    meshData.material.fragmentShaderPath + shaderExt, ShaderType::FRAGMENT);
                fragmentShader->setShaderCompiler(rendererBackend->createShaderCompiler());

                auto material = std::make_unique<Material>();
                material->setShaderProgram(rendererBackend->createShaderProgram());
                material->setVertexShader(std::move(vertexShader));
                material->setFragmentShader(std::move(fragmentShader));
                material->setBaseColor(meshData.material.color);

                if (!material->init()) {
                    LOG_ERROR("Material init failed for mesh: " + std::string(meshData.objPath));
                    continue;
                }

                auto meshRenderer = std::make_unique<MeshRenderer>();
                meshRenderer->setMaterial(std::move(material));

                gameObject->setMesh(std::move(mesh));
                gameObject->setMeshRenderer(std::move(meshRenderer));
            }
        }

        objects->push_back(gameObject);
    }

    return objects;
}

std::vector<Light>* SceneLoader::loadLights(const std::string& filepath) {
    if (!validateSceneFile(filepath)) return nullptr;

    std::ifstream file(filepath, std::ios::binary);
    CompiledScene scene;
    file.read(reinterpret_cast<char*>(&scene), sizeof(CompiledScene));
    
    auto lights = new std::vector<Light>();
    for (uint32_t i = 0; i < scene.lightCount; i++) {
        Light light;
        light.type = static_cast<LightType>(scene.lights[i].type);
        light.direction = scene.lights[i].direction;
        lights->push_back(light);
    }

    return lights;
}

bool SceneLoader::validateSceneFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.good()) {
        LOG_ERROR("Scene file does not exist: " + filepath);
        return false;
    }
    return true;
}