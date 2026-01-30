#ifndef SHADER_PROGRAM_FACTORY_HPP
#define SHADER_PROGRAM_FACTORY_HPP

#include "shader_program.hpp"
#include "graphics_api.hpp"
#include <memory>

class ShaderProgramFactory {
public:
    static std::unique_ptr<ShaderProgram> create(GraphicsAPI api, void* context = nullptr);
};

#endif // SHADERPROGRAMFACTORY_HPP
