#pragma once

#include <initializer_list>
#include <string>
#include <GL/gl.h>
#include <vector>

typedef std::string ShaderSource;
typedef GLuint Shader;
typedef GLuint ShaderProgram;
typedef GLint ShaderStatus;
typedef GLenum GlewError;
typedef GLenum ShaderType;

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

        ShaderLoader(ShaderLoader&& other) noexcept;
        ShaderLoader& operator=(ShaderLoader&& other) noexcept;

        static bool init();
        static std::string glew_get_error();

        void load(ShaderType type, std::initializer_list<std::string> sources);
        void use();
        void compile();

    private:
        static GlewError glew_error;
        static bool initialized;

        ShaderProgram program;
        std::vector<ShaderResource> shaders;

        void link();
        void cleanup_shaders();
};
