#define CLASS_NAME "Renderer"
#include "../log_macros.hpp"

#include "../game_object.hpp"
#include "../material.hpp"
#include "log_macros.hpp"
#include "renderer.hpp"
#include "renderer_factory.hpp"
#include <cstdint>
#include <cstdio>


Renderer::~Renderer() {
    if (backend) {
        delete backend;
    }
}

void Renderer::setRendererBackend(RendererBackend* backend) { this->backend = backend; }

RendererBackend* Renderer::getRendererBackend() { return backend; }

bool Renderer::initBackend(const GraphicsAPI& graphicsApi) {
    backend = RendererFactory::create(graphicsApi);
    if (!backend) {
        LOG_ERROR("Unsupported graphics API!");
        return false;
    }

    return true;
}

bool Renderer::initWindow(SDL_Window* win) {
    if (backend) {
        return backend->init(win);
    }

    return false;
}

void Renderer::renderMesh(const Mesh& mesh) { this->backend->draw(mesh); }

void Renderer::clearScreen() { return; }

void Renderer::renderGameObject(GameObject& gameObject) {

    auto mesh = gameObject.getMesh();
    if (!mesh) {
        LOG_INFO("Mesh is null!");
        return;
    }

    auto meshRenderer = gameObject.getMeshRenderer();
    if (!meshRenderer) {
        LOG_INFO("MeshRenderer is null!");
        return;
    }

    auto mat = meshRenderer->getMaterial();
    if (!mat) {
        LOG_INFO("Material is null!");
        return;
    }

    mat->use();

    auto program = mat->getProgram();
    if (program && program->isValid()) {
        auto value = reinterpret_cast<std::uintptr_t>(program->getHandle());
        unsigned int shader = static_cast<unsigned int>(value);
        backend->setUniforms(program);

        auto& lights = backend->getLights();
        if (!lights.empty()) {
            program->setUniformBuffer("LightData", 2, &lights[0].direction, sizeof(float) * 3);
        }
    } else {
        LOG_ERROR("Invalid shader program");
        return;
    }

    renderMesh(*mesh);
}

void Renderer::render(const Scene& scene) {

    if (!backend) {
        LOG_ERROR("Can not render without a renderer backend!");
    }

    if (!scene.getCamera()) {
        LOG_WARN("Scene doesn't have a main camera to render!");
        return;
    }

    backend->bindCamera(scene.getCamera());

    backend->clear(scene.getCamera());

    // Passar lights para o backend
    auto sceneLights = scene.getLights();
    std::vector<Light> lightsCopy;
    for (auto light : *sceneLights) {
        lightsCopy.push_back(light);
    }
    backend->setLights(lightsCopy);


    auto gos = scene.getGameObjects();
    for (const auto go : *gos) {
        renderGameObject(*go);
    }
}

void Renderer::render(const std::vector<GameObject*>* objects) {
    for (const auto go : *objects) {
        renderGameObject(*go);
    }

    if (backend && backend->getCamera()) {
        auto skybox = backend->getCamera()->getSkybox();
        if (skybox) {
            printf("Skybox exists\n");
            if (skybox->getMaterial()) {
                printf("Skybox material exists\n");
                skybox->getMaterial()->use();
                auto program = skybox->getMaterial()->getProgram();
                if (program) {
                    printf("Skybox program exists\n");
                    unsigned int shaderProgram = static_cast<unsigned int>(
                        reinterpret_cast<uintptr_t>(program->getHandle()));
                    printf("Calling renderSkybox with shader: %d, texture: %d\n", shaderProgram,
                           skybox->getTextureID());
                    backend->renderSkybox(*skybox->getMesh(), shaderProgram,
                                          skybox->getTextureID());
                } else {
                    printf("Skybox program is null\n");
                }
            } else {
                printf("Skybox material is null\n");
            }
        } else {
            // printf("Skybox is null\n");
        }
    } else {
        printf("Backend or camera is null\n");
    }
}