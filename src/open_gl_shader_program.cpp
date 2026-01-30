#include "open_gl_shader_program.hpp"
#include "shader_asset.hpp"
#include <cstdint>

OpenGLShaderProgram::~OpenGLShaderProgram() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

bool OpenGLShaderProgram::attachShader(const ShaderAsset& shader) {
    if (programID == 0) {
        programID = glCreateProgram();
    }

    auto value = reinterpret_cast<std::uintptr_t>(shader.getHandle());
    GLuint shaderID = static_cast<GLuint>(value);
    glAttachShader(programID, shaderID);
    return true;
}

bool OpenGLShaderProgram::link() {
    glLinkProgram(programID);
    
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    return success == GL_TRUE;
}

void OpenGLShaderProgram::use() {
    glUseProgram(programID);
}
