#include <GL/gl.h>
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
        GLuint texture;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, instance.monitor->stride / instance.monitor->channels);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            instance.monitor->alpha ? GL_RGBA8 : GL_RGB8,
            instance.monitor->width,
            instance.monitor->height,
            0,
            instance.monitor->alpha ? GL_RGBA : GL_RGB,
            GL_UNSIGNED_BYTE,
            instance.monitor->framebuffer
        );

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

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

    GLuint *BackgroundTexture::get_texture(Instance &instance)
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
            glDeleteTextures(1, &texture);
        }
    }

}
