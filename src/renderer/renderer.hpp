#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "../game_object.hpp"
#include "../mesh.hpp"
#include "renderer_backend.hpp"

class Material;

class Renderer{
private: 
    RendererBackend* backend = nullptr;

public:
    void setRendererBackend(RendererBackend* backend);
    RendererBackend* getRendererBackend();
    bool initContext();
    bool init();
    void preRender();
    void render(const std::vector<GameObject*>& objects);
    void renderMesh(const Mesh& mesh);
    void renderGameObject(GameObject& gameObject);
    void clearScreen();
};

#endif // RENDERER_HPP
