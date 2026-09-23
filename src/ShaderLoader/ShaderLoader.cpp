#include <GLFW/glfw3.h>
#include <utility>
#include <initializer_list>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <iostream>

#include "ShaderLoader.h"

#include "glbinding/gl/functions.h"
#include "glbinding/gl/types.h"
#include "glbinding/glbinding.h"
#include <glbinding/gl/gl.h>

bool ShaderLoader::initialized = false;
GLError ShaderLoader::gl_error = gl::GL_NO_ERROR;

bool ShaderLoader::init()
{
    if (initialized) return true;

    glbinding::initialize(glfwGetProcAddress);

    if (gl::glGetError() != gl::GL_NO_ERROR)
    {
        gl_error = gl::glGetError();
        return false;
    }

    initialized = true;
    return true;
}

std::string ShaderLoader::gl_get_error()
{
    return std::to_string(static_cast<int>(gl_error));
}

ShaderLoader::ShaderLoader()
{
    if (!initialized) throw std::runtime_error("Glew not initialized");
    program = gl::glCreateProgram();

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
        Shader shader = gl::glCreateShader(shader_res.type);

        if (!shader) {
            cleanup_shaders();
            throw std::runtime_error("Cannot create OpenGL shader");
        }

        const char* src = shader_res.source.c_str();
        gl::glShaderSource(shader, 1, &src, nullptr);
        gl::glCompileShader(shader);

        ShaderStatus succes;
        gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &succes);

        if (!succes)
        {
            gl::GLint len = 0;
            gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &len);
            cleanup_shaders();
            if (len > 0)
            {
                std::string log(len, '\0');
                gl::glGetShaderInfoLog(shader, len, &len, &log[0]);
                throw std::runtime_error("Shader compilation error: " + log);
            }
            throw std::runtime_error("Shader compilation error (log is empty)");
        }

        gl::glAttachShader(program, shader);
        shader_res.shader = shader;
    }
    link();
    cleanup_shaders();
}

void ShaderLoader::link()
{
    gl::glLinkProgram(program);

    ShaderStatus success;
    gl::glGetProgramiv(program, gl::GL_LINK_STATUS, &success);

    if (!success)
    {
        gl::GLint len = 0;
        gl::glGetProgramiv(program, gl::GL_INFO_LOG_LENGTH, &len);
        cleanup_shaders();
        if (len > 0)
        {
            std::string log(len, '\0');
            gl::glGetProgramInfoLog(program, len, &len, &log[0]);
            throw std::runtime_error("Program linking error: " + log);
        }
        throw std::runtime_error("Program linking error (log is empty)");
    }
}

void ShaderLoader::cleanup_shaders()
{
    for (auto &shader_res : shaders)
    {
        if (shader_res.shader) gl::glDeleteShader(shader_res.shader);
    }
    shaders.clear();
}

void ShaderLoader::use()
{
    gl::glUseProgram(program);
}

ShaderProgram ShaderLoader::get()
{
    return program;
}

ShaderLoader::~ShaderLoader()
{
    std::cout << "shaderloader destroyed" << std::endl;
    if (!program) return;
    if (shaders.size() > 0) cleanup_shaders();
    gl::glDeleteProgram(program);
}
