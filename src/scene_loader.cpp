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
    return basePath;
}

std::unique_ptr<Mesh> SceneLoader::loadObjMesh(const std::string& filepath, GraphicsAPI api) {
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

    auto mesh = std::make_unique<Mesh>(api);
    mesh->setVertices(vertices);
    mesh->setNormals(normals);
    return mesh;
}

std::unique_ptr<Camera> SceneLoader::loadCamera(const std::string& filepath,
                                                RendererBackend& rendererBackend) {
    std::ifstream file(filepath, std::ios::binary);
    CompiledScene scene;
    file.read(reinterpret_cast<char*>(&scene), sizeof(CompiledScene));

    auto camera = std::make_unique<Camera>();
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

std::vector<std::unique_ptr<GameObject>> SceneLoader::loadMeshes(const std::string& filepath, GraphicsAPI api, RendererBackend* backend) {
    std::ifstream file(filepath, std::ios::binary);
    CompiledScene scene;
    file.read(reinterpret_cast<char*>(&scene), sizeof(CompiledScene));

    std::vector<std::unique_ptr<GameObject>> objects;

    for (uint32_t i = 0; i < scene.meshCount; i++) {
        auto& meshData = scene.meshes[i];

        auto mesh = loadObjMesh(meshData.objPath, api);
        if (!mesh || !mesh->configure(backend)) {
            LOG_ERROR("Failed to load mesh: " + meshData.objPath);
            continue;
        }

        auto vertexShader = std::make_unique<ShaderAsset>(
            getShaderPath(meshData.material.vertexShaderPath, api), ShaderType::VERTEX, api, backend);
        auto fragmentShader = std::make_unique<ShaderAsset>(
            getShaderPath(meshData.material.fragmentShaderPath, api), ShaderType::FRAGMENT, api, backend);

        auto material = std::make_unique<Material>(api);
        material->setVertexShader(std::move(vertexShader));
        material->setFragmentShader(std::move(fragmentShader));
        material->setBaseColor(meshData.material.color);
        if (!material->init()) {
            LOG_ERROR("Material init failed for mesh: " + std::string(meshData.objPath));
            LOG_ERROR("Vertex shader: " + std::string(meshData.material.vertexShaderPath));
            LOG_ERROR("Fragment shader: " + std::string(meshData.material.fragmentShaderPath));
            continue;
        }

        auto meshRenderer = std::make_unique<MeshRenderer>();
        meshRenderer->setMaterial(std::move(material));

        auto gameObject = std::make_unique<GameObject>();
        gameObject->setMesh(std::move(mesh));
        gameObject->setMeshRenderer(std::move(meshRenderer));

        objects.push_back(std::move(gameObject));
    }

    return objects;
}

std::vector<Light> SceneLoader::loadLights(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    CompiledScene scene;
    file.read(reinterpret_cast<char*>(&scene), sizeof(CompiledScene));

    std::vector<Light> lights;
    for (uint32_t i = 0; i < scene.lightCount; i++) {
        Light light;
        light.type = static_cast<LightType>(scene.lights[i].type);
        light.direction = scene.lights[i].direction;
        lights.push_back(light);
    }
    return lights;
}