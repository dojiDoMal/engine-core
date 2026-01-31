#define CLASS_NAME "OpenGLShaderProgram"
#include "open_gl_shader_program.hpp"
#include "shader_asset.hpp"
#include <cstdint>
#include "log_macros.hpp"

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
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        LOG_ERROR("Shader program link error: " + infoLog);
    }
    return success == GL_TRUE;
}

void OpenGLShaderProgram::use() {
    glUseProgram(programID);
}

void OpenGLShaderProgram::setUniformBuffer(const char* name, int binding, const void* data, size_t size) {
    GLuint location = glGetUniformBlockIndex(programID, ("type_" + std::string(name)).c_str());
    if (location != GL_INVALID_INDEX) {
        glUniformBlockBinding(programID, location, binding);
        
        static GLuint ubo = 0;
        if (ubo == 0) {
            glGenBuffers(1, &ubo);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
    }
    LOG_WARN("Uniform has invalid index!");
}
