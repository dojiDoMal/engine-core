#include "shader_asset.hpp"
#include "shader_compiler_factory.hpp"
#include <fstream>

ShaderAsset::ShaderAsset(const std::string& path, ShaderType type, GraphicsAPI api, void* context) 
    : Asset(path), shaderType(type) {
    compiler = ShaderCompilerFactory::create(api, context);
}

bool ShaderAsset::load() {
    if (compiler && compiler->compile(getPath(), shaderType, &shaderHandle)) {
        loaded = true;
        return true;
    }
    
    printf("ERROR: Shader compilation failed for: %s\n", getPath().c_str());
    return false;
}

void ShaderAsset::unload() {
    if (compiler && shaderHandle) {
        compiler->destroy(shaderHandle);
        shaderHandle = nullptr;
    }
    loaded = false;
}
