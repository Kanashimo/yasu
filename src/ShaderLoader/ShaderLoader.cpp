#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <utility>
#include <initializer_list>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <iostream>

#include "ShaderLoader.h"

bool ShaderLoader::initialized = false;
GlewError ShaderLoader::glew_error = GLEW_OK;

bool ShaderLoader::init()
{
    if (initialized) return true;

    glewExperimental = GL_TRUE;
    GlewError glew = glewInit();

    if (glew != GLEW_OK)
    {
        glew_error = glew;
        return false;
    }

    initialized = true;
    return true;
}

std::string ShaderLoader::glew_get_error()
{
    return std::string(reinterpret_cast<const char*>(glewGetErrorString(glew_error)));
}

ShaderLoader::ShaderLoader()
{
    if (!initialized) throw std::runtime_error("Glew not initialized");
    program = glCreateProgram();

    if (!program)
    {
        throw std::runtime_error("Cannot create OpenGL program");
    }
}

void ShaderLoader::load(ShaderType type, std::initializer_list<std::string> sources)
{
    for (auto &source : sources) {
        ShaderResource shader_res;

        std::ifstream file(source);

        if (!file.is_open())
        {
            cleanup_shaders();
            throw std::runtime_error("Cannot open shader file: " + source);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        shader_res.source = buffer.str();
        shader_res.type = type;
        shaders.push_back(std::move(shader_res));
    }
}

void ShaderLoader::compile()
{
    for (auto &shader_res : shaders)
    {
        Shader shader = glCreateShader(shader_res.type);

        if (!shader) {
            cleanup_shaders();
            throw std::runtime_error("Cannot create OpenGL shader");
        }

        const char* src = shader_res.source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        ShaderStatus succes;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &succes);

        if (!succes)
        {
            GLint len = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
            cleanup_shaders();
            if (len > 0)
            {
                std::string log(len, '\0');
                glGetShaderInfoLog(shader, len, &len, &log[0]);
                throw std::runtime_error("Shader compilation error: " + log);
            }
            throw std::runtime_error("Shader compilation error (log is empty)");
        }

        glAttachShader(program, shader);
        shader_res.shader = shader;
    }
    link();
    cleanup_shaders();
}

void ShaderLoader::link()
{
    glLinkProgram(program);

    ShaderStatus success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        cleanup_shaders();
        if (len > 0)
        {
            std::string log(len, '\0');
            glGetProgramInfoLog(program, len, &len, &log[0]);
            throw std::runtime_error("Program linking error: " + log);
        }
        throw std::runtime_error("Program linking error (log is empty)");
    }
}

void ShaderLoader::cleanup_shaders()
{
    for (auto &shader_res : shaders)
    {
        if (shader_res.shader) glDeleteShader(shader_res.shader);
    }
    shaders.clear();
}

void ShaderLoader::use()
{
    glUseProgram(program);
}

ShaderLoader::~ShaderLoader()
{
    std::cout << "shaderloader destroyed" << std::endl;
    if (!program) return;
    if (shaders.size() > 0) cleanup_shaders();
    glDeleteProgram(program);
}
