#include <GL/glew.h>
#include <GL/gl.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <fstream>

#include "ShaderLoader.h"

// TODO: refactor everything, use classes

typedef std::string ShaderSource;
typedef GLuint Shader;

ShaderSource ShaderLoader::load(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open shader file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

Shader ShaderLoader::compile(GLenum type, ShaderSource src)
{
    Shader shader = glCreateShader(type);

    const char* c_src = src.c_str();
    glShaderSource(shader, 1, &c_src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(shader, len, &len, &log[0]);
        throw std::runtime_error("Shader compilation error: " + log);
    }

    return shader;
}

void ShaderLoader::attach(GLuint program, Shader shader) {
    glAttachShader(program, shader);
}

void ShaderLoader::link(GLuint program)
{
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(program, len, &len, &log[0]);
        throw std::runtime_error("Program linking error: " + log);
    }
}
