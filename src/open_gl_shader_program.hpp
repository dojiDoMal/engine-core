#ifndef OPEN_GL_SHADER_PROGRAM_HPP
#define OPEN_GL_SHADER_PROGRAM_HPP

#include "shader_program.hpp"
#include <GL/glew.h>

class OpenGLShaderProgram : public ShaderProgram {
private:
    GLuint programID = 0;

public:
    ~OpenGLShaderProgram() override;
    bool attachShader(const ShaderAsset& shader) override;
    bool link() override;
    void use() override;
    void* getHandle() const override { return reinterpret_cast<void*>(programID); }
    bool isValid() const override { return programID != 0; }
};

#endif // OPENGLSHADERPROGRAM_HPP
