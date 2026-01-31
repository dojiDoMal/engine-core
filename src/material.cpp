#include "material.hpp"
#include "color.hpp"
#include "shader_program_factory.hpp"

Material::Material(GraphicsAPI api) : api(api) {
    shaderProgram = ShaderProgramFactory::create(api);
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

    shaderProgram->setUniformBuffer("MaterialData", 1, &baseColor, sizeof(baseColor));
    return true;
}

void Material::setContext(void* context) {
    if (api == GraphicsAPI::DIRECTX11){
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
        shaderProgram->setUniformBuffer("MaterialData", 1, &baseColor, sizeof(baseColor));
    }
}