#ifndef OPEN_GL_RENDERER_BACKEND_HPP
#define OPEN_GL_RENDERER_BACKEND_HPP

#include <GL/glew.h>
#include "../../../graphics_api.hpp"
#include "../../renderer_backend.hpp"
#include "../../../mesh.hpp"
#include <vector>
#include <string>

class OpenGLRendererBackend : public RendererBackend {
private: 
    GLuint matricesUBO = 0;
    GLuint lightDataUBO = 0;

public:
    ~OpenGLRendererBackend();
    bool init() override;
    bool initWindowContext() override;
    void bindCamera(Camera* camera) override {return;};
    void applyMaterial(Material* material) override {};
    void renderGameObjects(std::vector<GameObject*>* gameObjects, std::vector<Light>* lights) override {};
    void setBufferDataImpl(const std::string& name, const void* data, size_t size) override {};
    void clear(Camera* camera) override;  
    void draw(const Mesh&) override;
    void setUniforms(ShaderProgram* shaderProgram) override;
    void onCameraSet() override;

    // Skybox management
    unsigned int createCubemapTexture(const std::vector<std::string>& faces) override;
    GraphicsAPI getGraphicsAPI() const override;
    void deleteCubemapTexture(unsigned int textureID);
    void renderSkybox(const Mesh& mesh, unsigned int shaderProgram, unsigned int textureID) override;

    unsigned int getRequiredWindowFlags() const override;
    bool init(SDL_Window* window) override;
};

#endif // OPENGLRENDERERBACKEND_HPP
