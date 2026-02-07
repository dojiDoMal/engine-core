#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include "mesh.hpp"
#include "mesh_renderer.hpp"
#include <memory>

class GameObject {
  private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<MeshRenderer> meshRenderer;

  public:
    GameObject() = default;
    void setMesh(std::unique_ptr<Mesh> m) { mesh = std::move(m); };
    Mesh* getMesh() { return mesh.get(); }
    const Mesh* getMesh() const { return mesh.get(); }
    bool hasMesh() const { return mesh != nullptr; }

    void setMeshRenderer(std::unique_ptr<MeshRenderer> m) { meshRenderer = std::move(m); };
    MeshRenderer* getMeshRenderer() { return meshRenderer.get(); }
    const MeshRenderer* getMeshRenderer() const { return meshRenderer.get(); }
    bool hasMeshRenderer() const { return meshRenderer != nullptr; }
};

#endif // GAMEOBJECT_HPP