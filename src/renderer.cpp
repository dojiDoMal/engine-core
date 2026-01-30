#include "renderer.hpp"
#include "game_object.hpp"
#include "material.hpp"
#include <cstdint>
#include <cstdio>

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
        printf("gameobject does not have a mesh!\n");
        return;
    }

    auto meshRenderer = gameObject.getMeshRenderer();
    if (!meshRenderer) {
        printf("gameobject does not have a meshrenderer!\n");
        return;
    }

    auto mat = meshRenderer->getMaterial();
    if (!mat) {
        printf("gameobject meshrenderer does not have a material!\n");
        return;
    }
    mat->use();

    auto program = mat->getProgram();
    if (program && program->isValid()) {
        auto value = reinterpret_cast<std::uintptr_t>(program->getHandle());
        unsigned int shader = static_cast<unsigned int>(value);
        backend->setUniforms(shader);
    } else {
        printf("Error: Invalid shader program\n");
        return;
    }
    
    renderMesh(*mesh);
}

void Renderer::render(const std::vector<GameObject*>& objects) {
    for (const auto& go : objects) {
        renderGameObject(*go);
    }

    //printf("Checking skybox...\n");
    if (backend && backend->getCamera()) {
        //printf("Backend and camera exist\n");
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