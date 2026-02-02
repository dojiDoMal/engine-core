#ifndef RENDERER_BACKEND_HPP
#define RENDERER_BACKEND_HPP

#include "../graphics_api.hpp"
#include "../mesh.hpp"
#include "../camera.hpp"
#include "../light.hpp"
#include <vector>

struct SDL_Window;

class RendererBackend{
protected:
    Camera* mainCamera = nullptr;
    std::vector<Light> lights;

public:
    virtual ~RendererBackend() = default;

    virtual bool init() = 0;
    virtual bool initWindowContext() = 0;
    virtual void bindCamera(Camera* camera) = 0;
    virtual void clear(Camera* camera) = 0;
    virtual void draw(const Mesh&) = 0;
    virtual unsigned int createCubemapTexture(const std::vector<std::string>& faces) = 0;
    virtual GraphicsAPI getGraphicsAPI() const = 0;
    virtual void onCameraSet() = 0;
    virtual void setUniforms(void* shaderProgram) = 0;
    virtual void renderSkybox(const Mesh& mesh, unsigned int shaderProgram, unsigned int textureID) = 0;
    
    virtual unsigned int getRequiredWindowFlags() const = 0;
    virtual bool init(SDL_Window* window) = 0;
    
    Camera* getCamera(){ return mainCamera; }

    void setCamera(Camera* camera) {
        mainCamera = camera;
        onCameraSet();
    }

    void setLights(const std::vector<Light>& sceneLights) { lights = sceneLights; }
    const std::vector<Light>& getLights() const { return lights; }
};

#endif // RENDERERBACKEND_HPP
