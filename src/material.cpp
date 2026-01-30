#include "material.hpp"
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
    
    return shaderProgram->link();
}

void Material::setContext(void* context) {
    if (api == GraphicsAPI::DIRECTX11){
        shaderProgram = ShaderProgramFactory::create(api, context);
    }
}