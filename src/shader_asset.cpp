#define CLASS_NAME "ShaderAsset"
#include "shader_asset.hpp"
#include "shader_compiler_factory.hpp"
#include <fstream>
#include "log_macros.hpp"

ShaderAsset::ShaderAsset(const std::string& path, ShaderType type, GraphicsAPI api, void* context) 
    : Asset(path), shaderType(type) {
    compiler = ShaderCompilerFactory::create(api, context);
}

bool ShaderAsset::load() {
    if (compiler && compiler->compile(getPath(), shaderType, &shaderHandle)) {
        loaded = true;
        return true;
    }
    
    LOG_ERROR("Shader compilation failed for: " + getPath());
    return false;
}

void ShaderAsset::unload() {
    if (compiler && shaderHandle) {
        compiler->destroy(shaderHandle);
        shaderHandle = nullptr;
    }
    loaded = false;
}
