#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "color.hpp"
#include "graphics_api.hpp"
#include "light.hpp"
#include "shader_asset.hpp"
#include "shader_program.hpp"
#include <memory>

class Material
{
private:
    std::unique_ptr<ShaderAsset> vertexShader;
    std::unique_ptr<ShaderAsset> fragmentShader;
    std::unique_ptr<ShaderProgram> shaderProgram;
    GraphicsAPI api;
    ColorRGBA baseColor = COLOR::GREEN;

public:

    Material(GraphicsAPI api);

    bool init();
    void use();
    void setContext(void* context);
    void setBaseColor(const ColorRGBA color);
    void applyLight(const Light light);

    void setVertexShader(std::unique_ptr<ShaderAsset> shader) {
        vertexShader = std::move(shader);
    }
    
    void setFragmentShader(std::unique_ptr<ShaderAsset> shader) {
        fragmentShader = std::move(shader);
    }
     
    ShaderProgram* getProgram() const { return shaderProgram.get(); }
};

#endif // MATERIAL_HPP