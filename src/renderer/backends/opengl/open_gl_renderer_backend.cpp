#define CLASS_NAME "OpenGLRendererBackend"
#include "open_gl_renderer_backend.hpp"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "color.hpp"
#include "graphics_api.hpp"
#include "stb_image.h"
#include "log_macros.hpp"

GraphicsAPI OpenGLRendererBackend::getGraphicsAPI() const {
    return GraphicsAPI::OPENGL;
}

OpenGLRendererBackend::~OpenGLRendererBackend() {
    if (matricesUBO) glDeleteBuffers(1,&matricesUBO);
    if (lightDataUBO) glDeleteBuffers(1, &lightDataUBO);
}

void OpenGLRendererBackend::onCameraSet() {
    if (mainCamera) {
        ColorRGBA& bgColor = mainCamera->getBackgroundColor();
        glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    }
}

bool OpenGLRendererBackend::init() 
{
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::string glewErr = reinterpret_cast<const char*>(glewGetErrorString(err));
        LOG_ERROR("GLEW initialization failed: " + glewErr);
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_FRAMEBUFFER_SRGB);
    
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    return true;
}

void OpenGLRendererBackend::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool OpenGLRendererBackend::initWindowContext() 
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    return true;
}

void OpenGLRendererBackend::draw(const Mesh& mesh)
{
    glBindVertexArray(mesh.getVAO());
    glDrawArrays(GL_TRIANGLES, 0, mesh.getVertices().size() / 3);
    glBindVertexArray(0);
}

void OpenGLRendererBackend::setUniforms(void* shaderProgram) {

    if (!mainCamera) {
        LOG_ERROR("mainCamera is null");
        return;
    }

    glm::mat4 model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 1.0f));
    model = glm::rotate(model, SDL_GetTicks() / 1000.0f, glm::vec3(0.5f, 1.0f, 0.0f));

    auto& camPos = mainCamera->getPosition();
    glm::mat4 view = glm::lookAt(
        {camPos.x, camPos.y, camPos.z},
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(mainCamera->getFov()), 
        mainCamera->getAspectRatio(), 
        mainCamera->getNearDistance(), 
        mainCamera->getFarDistance());

    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

unsigned int OpenGLRendererBackend::createCubemapTexture(const std::vector<std::string>& faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            LOG_WARN("Cubemap texture failed to load at path: " + faces[i].c_str());
            stbi_image_free(data);
            return 0;
        }
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void OpenGLRendererBackend::deleteCubemapTexture(unsigned int textureID) {
    glDeleteTextures(1, &textureID);
}

void OpenGLRendererBackend::renderSkybox(const Mesh& mesh, unsigned int shaderProgram, unsigned int textureID) {
    if (!mainCamera) return;

    glDepthFunc(GL_LEQUAL);
    
    auto& camPos = mainCamera->getPosition();
    glm::mat4 camView = glm::lookAt(
        {camPos.x, camPos.y, camPos.z},
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    
    glm::mat4 view = glm::mat4(glm::mat3(camView));
    glm::mat4 projection = glm::perspective(
        glm::radians(mainCamera->getFov()), 
        mainCamera->getAspectRatio(), 
        mainCamera->getNearDistance(), 
        mainCamera->getFarDistance());
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(shaderProgram, "skybox"), 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    glBindVertexArray(mesh.getVAO());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    glDepthFunc(GL_LESS);
}
