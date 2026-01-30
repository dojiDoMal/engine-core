#ifndef SHADER_COMPILER_HPP
#define SHADER_COMPILER_HPP

#include <string>

enum class ShaderType {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    COMPUTE
};

class ShaderCompiler {
public:
    virtual ~ShaderCompiler() = default;
    virtual bool compile(const std::string& source, ShaderType type, void** outHandle) = 0;
    virtual void destroy(void* handle) = 0;
    virtual bool isValid(void* handle) = 0;
};

#endif // SHADERCOMPILER_HPP
