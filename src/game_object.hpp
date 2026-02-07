#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include "mesh.hpp"
#include "mesh_renderer.hpp"
#include "sprite.hpp"
#include "sprite_renderer.hpp"
#include <memory>

class GameObject {
  private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<MeshRenderer> meshRenderer;
    std::unique_ptr<Sprite> sprite;
    std::unique_ptr<SpriteRenderer> spriteRenderer;

  public:
    GameObject() = default;

    void setMesh(std::unique_ptr<Mesh> m) { mesh = std::move(m); }
    Mesh* getMesh() { return mesh.get(); }
    const Mesh* getMesh() const { return mesh.get(); }
    bool hasMesh() const { return mesh != nullptr; }

    void setMeshRenderer(std::unique_ptr<MeshRenderer> m) { meshRenderer = std::move(m); }
    MeshRenderer* getMeshRenderer() { return meshRenderer.get(); }
    const MeshRenderer* getMeshRenderer() const { return meshRenderer.get(); }
    bool hasMeshRenderer() const { return meshRenderer != nullptr; }

    void setSprite(std::unique_ptr<Sprite> s) { sprite = std::move(s); }
    Sprite* getSprite() { return sprite.get(); }
    const Sprite* getSprite() const { return sprite.get(); }
    bool hasSprite() const { return sprite != nullptr; }

    void setSpriteRenderer(std::unique_ptr<SpriteRenderer> sr) { spriteRenderer = std::move(sr); }
    SpriteRenderer* getSpriteRenderer() { return spriteRenderer.get(); }
    const SpriteRenderer* getSpriteRenderer() const { return spriteRenderer.get(); }
    bool hasSpriteRenderer() const { return spriteRenderer != nullptr; }
};

#endif
