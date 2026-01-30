#ifndef SHADER_ASSET_HPP
#define SHADER_ASSET_HPP

#include "graphics_api.hpp"
#include "asset.hpp"
#include "shader_compiler.hpp"
#include <memory>

class ShaderAsset : public Asset
{
private:
    void* shaderHandle = nullptr;
    ShaderType shaderType;
    std::unique_ptr<ShaderCompiler> compiler;
    bool isCompiled = false;

public:
    ShaderAsset(const std::string& path, ShaderType type, GraphicsAPI api, void* context = nullptr);
    ~ShaderAsset() override { unload(); }
    
    bool load() override;
    void unload() override;
    
    void* getHandle() const { return shaderHandle; }
    ShaderType getType() const { return shaderType; }
};

#endif // SHADERASSET_HPP
