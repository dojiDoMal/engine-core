#define CLASS_NAME "Renderer"
#include "renderer.hpp"
#include "game_object.hpp"
#include "material.hpp"
#include <cstdint>
#include <cstdio>
#include "log_macros.hpp"

void Renderer::setRendererBackend(RendererBackend *backend) 
{ 
    this->backend = backend; 
}

RendererBackend* Renderer::getRendererBackend() 
{ 
    return backend; 
}

bool Renderer::initContext() 
{
    if (backend) 
    {
        backend->initWindowContext();
    }
    
    return true;
}

bool Renderer::init() 
{
    if (backend)
    {
        backend->init();
    }

    return true;
}

void Renderer::renderMesh(const Mesh &mesh) 
{ 
    this->backend->draw(mesh); 
}

void Renderer::clearScreen() 
{
    this->backend->clear(); 
}

void Renderer::renderGameObject(GameObject& gameObject) {
    auto mesh = gameObject.getMesh();
    if (!mesh) {
        LOG_INFO((std::string)"Mesh is null!");
        return;
    }

    auto meshRenderer = gameObject.getMeshRenderer();
    if (!meshRenderer) {
        LOG_INFO((std::string)"MeshRenderer is null!");
        return;
    }

    auto mat = meshRenderer->getMaterial();
    if (!mat) {
        LOG_INFO((std::string)"Material is null!");
        return;
    }
    mat->use();

    auto program = mat->getProgram();
    if (program && program->isValid()) {
        auto value = reinterpret_cast<std::uintptr_t>(program->getHandle());
        unsigned int shader = static_cast<unsigned int>(value);
        backend->setUniforms(shader);
        
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

void Renderer::render(const std::vector<GameObject*>& objects) {
    for (const auto& go : objects) {
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
                    printf("Calling renderSkybox with shader: %d, texture: %d\n", shaderProgram, skybox->getTextureID());
                    backend->renderSkybox(*skybox->getMesh(), shaderProgram, skybox->getTextureID());
                } else {
                    printf("Skybox program is null\n");
                }
            } else {
                printf("Skybox material is null\n");
            }
        } else {
            //printf("Skybox is null\n");
        }
    } else {
        printf("Backend or camera is null\n");
    }
}