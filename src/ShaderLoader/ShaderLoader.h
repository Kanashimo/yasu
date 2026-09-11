#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <string>

typedef std::string ShaderSource;
typedef GLuint Shader;

namespace ShaderLoader
{
    ShaderSource load(const std::string& path);
    Shader compile(GLenum type, ShaderSource src);
    void attach(GLuint program, Shader shader);
    void link(GLuint program);
}
