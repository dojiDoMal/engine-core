#define CLASS_NAME "Material"
#include "log_macros.hpp"

#include "material.hpp"
#include "color.hpp"
#include "light.hpp"
#include "shader_program_factory.hpp"

Material::Material(GraphicsAPI api) : api(api) {
    if (api == GraphicsAPI::OPENGL || api == GraphicsAPI::WEBGL) {
        shaderProgram = ShaderProgramFactory::create(api);
    }
}

bool Material::init() {
    if (!vertexShader || !fragmentShader || !shaderProgram) {
        return false;
    }
    
    if (!vertexShader->load() || !fragmentShader->load()) {
        return false;
    }
    
    if (!shaderProgram->attachShader(*vertexShader) || 
        !shaderProgram->attachShader(*fragmentShader)) {
        return false;
    }
    
    if (!shaderProgram->link()) {
        return false;
    }

    setBaseColor(baseColor);
    return true;
}

void Material::setContext(void* context) {
    if (api == GraphicsAPI::DIRECTX12 || api == GraphicsAPI::VULKAN) {
        shaderProgram = ShaderProgramFactory::create(api, context);
    }
}

void Material::use() { 
    if(shaderProgram) {
        shaderProgram->use();
    } 
}

void Material::setBaseColor(const ColorRGBA color) {
    baseColor = color;
    if (shaderProgram) {
        shaderProgram->setUniformBuffer("MaterialData", &baseColor, sizeof(baseColor));
    }
}

void Material::applyLight(const Light light) {
    if (!shaderProgram) {
        LOG_WARN("Can not apply light on material with null shaderProgram");
        return;
    }

    if (light.type == LightType::DIRECTIONAL) {
        shaderProgram->setUniformBuffer("LightData", &light.direction, sizeof(light.direction));
    }
}