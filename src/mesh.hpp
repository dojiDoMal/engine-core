#ifndef MESH_HPP
#define MESH_HPP

#include "graphics_api.hpp"
#include "mesh_buffer.hpp"
#include <memory>
#include <vector>

class Mesh {
private:
  std::vector<float> vertices;
  std::vector<float> normals;
  std::unique_ptr<MeshBuffer> meshBuffer;
  GraphicsAPI api;

public:
  Mesh(GraphicsAPI graphicsAPI) : api(graphicsAPI) {}

  void setVertices(const std::vector<float> &v) { vertices = v; }
  void setNormals(const std::vector<float> &n) { normals = n; }
  const std::vector<float> &getVertices() const { return vertices; }
  const std::vector<float> &getNormals() const { return normals; }

  bool configure(void *context = nullptr);
  void bind() {
    if (meshBuffer)
      meshBuffer->bind();
  }
  void unbind() {
    if (meshBuffer)
      meshBuffer->unbind();
  }

  unsigned int getVAO() const {
    if ((api == GraphicsAPI::OPENGL || api == GraphicsAPI::WEBGL) &&
        meshBuffer) {
      return reinterpret_cast<uintptr_t>(meshBuffer->getHandle());
    }
    return 0;
  }

  void *getHandle() const {
    return meshBuffer ? meshBuffer->getHandle() : nullptr;
  }

  void *getMeshHandle() const {
    return meshBuffer ? meshBuffer->getHandle() : nullptr;
  }
};

#endif // MESH_HPP
