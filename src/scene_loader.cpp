#define CLASS_NAME "SceneLoader"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"
#include "scene_loader.hpp"
#include "scene_format.hpp"
#include "material.hpp"
#include "mesh_renderer.hpp"
#include "shader_asset.hpp"
#include "skybox.hpp"
#include "log_macros.hpp"

std::string SceneLoader::getShaderPath(const std::string& basePath, GraphicsAPI api) {
    if (api == GraphicsAPI::WEBGL || api == GraphicsAPI::OPENGL) {
        return basePath + ".glsl";
    }
    if (api == GraphicsAPI::VULKAN) {
        return basePath + ".spv";
    }
    if (api == GraphicsAPI::DIRECTX12) {
        return basePath + ".cso";
    }
    return basePath;
}

std::unique_ptr<Mesh> SceneLoader::loadObjMesh(const std::string& filepath, bool shadeSmooth, GraphicsAPI api) {
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
                float normal[3] = {
                    edge1[1] * edge2[2] - edge1[2] * edge2[1],
                    edge1[2] * edge2[0] - edge1[0] * edge2[2],
                    edge1[0] * edge2[1] - edge1[1] * edge2[0]
                };
                
                // Normalizar
                float len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
                if (len > 0) {
                    normal[0] /= len; normal[1] /= len; normal[2] /= len;
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

    auto mesh = std::make_unique<Mesh>(api);
    mesh->setVertices(vertices);
    mesh->setNormals(normals);
    return mesh;
}

Camera* SceneLoader::loadCamera(const std::string& filepath,
                                                RendererBackend& rendererBackend) {
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
        auto graphicsAPI = rendererBackend.getGraphicsAPI();
        auto skybox = std::make_unique<Skybox>(graphicsAPI);

        auto skyboxVertexShaderPtr = std::make_unique<ShaderAsset>(
            getShaderPath(cam.skybox.material.vertexShaderPath, graphicsAPI), ShaderType::VERTEX, graphicsAPI, &rendererBackend);
        auto skyboxFragmentShaderPtr = std::make_unique<ShaderAsset>(
            getShaderPath(cam.skybox.material.fragmentShaderPath, graphicsAPI), ShaderType::FRAGMENT, graphicsAPI, &rendererBackend);

        auto skyboxMaterial = std::make_unique<Material>(graphicsAPI);
        skyboxMaterial->setContext(&rendererBackend);
        skyboxMaterial->setVertexShader(std::move(skyboxVertexShaderPtr));
        skyboxMaterial->setFragmentShader(std::move(skyboxFragmentShaderPtr));
        skyboxMaterial->init();

        std::vector<std::string> faces;
        for (const auto row : cam.skybox.cubeMapTextures) {
            faces.push_back(row);
        }

        unsigned int cubemapID = rendererBackend.createCubemapTexture(faces);
        skybox->setTextureID(cubemapID);
        skybox->setMaterial(std::move(skyboxMaterial));
        skybox->init();
        camera->setSkybox(std::move(skybox));
    }

    return camera;
}

std::vector<GameObject*>* SceneLoader::loadGameObjects(const std::string& filepath, GraphicsAPI api, RendererBackend* backend) {
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
                
                auto mesh = loadObjMesh(meshData.objPath, meshData.shadeSmooth, api);
                if (!mesh || !mesh->configure(backend)) {
                    LOG_ERROR("Failed to load mesh: " + std::string(meshData.objPath));
                    continue;
                }

                auto vertexShader = std::make_unique<ShaderAsset>(
                    getShaderPath(meshData.material.vertexShaderPath, api), ShaderType::VERTEX, api, backend);
                auto fragmentShader = std::make_unique<ShaderAsset>(
                    getShaderPath(meshData.material.fragmentShaderPath, api), ShaderType::FRAGMENT, api, backend);

                auto material = std::make_unique<Material>(api);
                material->setContext(backend);
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