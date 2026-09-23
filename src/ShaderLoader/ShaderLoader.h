#pragma once

#include <glbinding/gl/gl.h>
#include <initializer_list>
#include <string>
#include <vector>

typedef std::string ShaderSource;
typedef gl::GLuint Shader;
typedef gl::GLuint ShaderProgram;
typedef gl::GLint ShaderStatus;
typedef gl::GLenum GLError;
typedef gl::GLenum ShaderType;

struct ShaderResource {
    ShaderSource source;
    ShaderType type;
    Shader shader;
};

class ShaderLoader
{
    public:
        ShaderLoader();
        ~ShaderLoader();

        ShaderLoader(const ShaderLoader&) = delete;
        ShaderLoader &operator=(ShaderLoader&) = delete;

        static bool init();
        static std::string gl_get_error();

        void load(ShaderType type, std::initializer_list<std::string> sources);
        void use();
        void compile();
        ShaderProgram get();

    private:
        static GLError gl_error;
        static bool initialized;

        ShaderProgram program;
        std::vector<ShaderResource> shaders;

        void link();
        void cleanup_shaders();
};
