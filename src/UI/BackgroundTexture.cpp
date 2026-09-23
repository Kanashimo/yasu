#include <GLFW/glfw3.h>
#include <imgui.h>
#include <algorithm>

#include "src/Application.h"
#include "src/UI/BackgroundTexture.h"

namespace UI {

    void BackgroundTexture::on_init()
    {
        begin_time = glfwGetTime();
    }

    void BackgroundTexture::on_instance_init(Instance &instance)
    {
        gl::GLuint texture;

        gl::glGenTextures(1, &texture);
        gl::glBindTexture(gl::GL_TEXTURE_2D, texture);

        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_LINEAR);
        gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_LINEAR);

        gl::glPixelStorei(gl::GL_UNPACK_ALIGNMENT, 1);
        gl::glPixelStorei(gl::GL_UNPACK_ROW_LENGTH, instance.monitor->stride / instance.monitor->channels);

        gl::glTexImage2D(
            gl::GL_TEXTURE_2D,
            0,
            instance.monitor->alpha ? gl::GL_RGBA8 : gl::GL_RGB8,
            instance.monitor->width,
            instance.monitor->height,
            0,
            instance.monitor->alpha ? gl::GL_RGBA : gl::GL_RGB,
            gl::GL_UNSIGNED_BYTE,
            instance.monitor->framebuffer
        );

        gl::glPixelStorei(gl::GL_UNPACK_ROW_LENGTH, 0);

        textures[&instance] = texture;
    }

    void BackgroundTexture::on_update(Instance &instance)
    {
        ImGui::GetBackgroundDrawList()->AddImage(
            (ImTextureID)(intptr_t)textures[&instance],
            ImVec2(0, 0),
            ImVec2((float)instance.monitor->width, (float)instance.monitor->height)
        );

        double current_time = glfwGetTime() - begin_time;

        fade = (float)current_time / 0.70f;
        fade = std::clamp(fade, 0.0f, 0.40f);

        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(0, 0),
            ImVec2((float)instance.monitor->width, (float)instance.monitor->height),
            IM_COL32(0, 0, 0, (int)(fade * 255.0f))
        );
    }

    gl::GLuint *BackgroundTexture::get_texture(Instance &instance)
    {
        if (textures.contains(&instance))
        {
            return &textures.at(&instance);
        }
        return nullptr;
    }

    BackgroundTexture::~BackgroundTexture()
    {
        for (const auto &[_, texture] : textures)
        {
            gl::glDeleteTextures(1, &texture);
        }
    }

}
