#include "Pipette.h"
#include "imgui.h"
#include "src/Application.h"
#include "src/UI/BackgroundTexture.h"
#include "src/UI/Toolbar.h"
// #include <GL/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdint>

namespace UI {

    void Pipette::on_update(Instance &instance)
    {
        if (Application::application().module<Toolbar>()->get_current_option() != OPTION_PIPETTE) return;

        pos = ImGui::GetMousePos();

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !dragging && !ImGui::GetIO().WantCaptureMouse)
        {
            dragging = true;
            glfwSetInputMode(instance.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
            Application::application().module<Toolbar>()->set_busy(true);
        }

        if (dragging && !pixel.empty())
        {
            gl::GLuint *texture = Application::application().module<BackgroundTexture>()->get_texture(instance);

            float radius = 20.0f;

            float u0 = (pos.x - radius) / instance.monitor->width;
            float u1 = (pos.x + radius) / instance.monitor->width;

            float v0 = (pos.y - radius) / instance.monitor->height;
            float v1 = (pos.y + radius) / instance.monitor->height;

            ImGui::GetForegroundDrawList()->AddImageRounded(
                (ImTextureID)(intptr_t)*texture,
                ImVec2(pos.x - 100.0f, pos.y - 100.0f),
                ImVec2(pos.x + 100.0f, pos.y + 100.0f),
                ImVec2(u0, v0),
                ImVec2(u1, v1),
                IM_COL32(255, 255, 255, 255),
                100.0f
            );

            ImGui::GetForegroundDrawList()->AddLine(
                ImVec2(pos.x + 5.0f, pos.y - 100.0f),
                ImVec2(pos.x + 5.0f, pos.y + 100.0f),
                IM_COL32(255, 255, 255, 125)
            );

            ImGui::GetForegroundDrawList()->AddLine(
                ImVec2(pos.x - 5.0f, pos.y - 100.0f),
                ImVec2(pos.x - 5.0f, pos.y + 100.0f),
                IM_COL32(255, 255, 255, 125)
            );

            ImGui::GetForegroundDrawList()->AddLine(
                ImVec2(pos.x - 100.0f, pos.y - 5.0f),
                ImVec2(pos.x + 100.0f, pos.y - 5.0f),
                IM_COL32(255, 255, 255, 125)
            );
            ImGui::GetForegroundDrawList()->AddLine(
                ImVec2(pos.x - 100.0f, pos.y + 5.0f),
                ImVec2(pos.x + 100.0f, pos.y + 5.0f),
                IM_COL32(255, 255, 255, 125)
            );

            ImGui::GetForegroundDrawList()->AddCircle(
                pos,
                100.0f,
                IM_COL32(pixel[0], pixel[1], pixel[2], 255),
                0,
                15.0f
            );

            ImGui::GetForegroundDrawList()->AddCircle(
                pos,
                107.5f,
                IM_COL32(255, 255, 255, 125),
                0,
                1.0f
            );

            ImGui::GetForegroundDrawList()->AddCircle(
                pos,
                92.5f,
                IM_COL32(255, 255, 255, 125),
                0,
                1.0f
            );
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            dragging = false;
            glfwSetInputMode(instance.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
            Application::application().module<Toolbar>()->set_busy(false);
        }
    }

    void Pipette::on_after_render(Instance &instance)
    {
        if (!dragging) return;
        if (pixel.empty()) pixel.resize(3);

        glReadPixels(
            (int)pos.x,
            instance.monitor->height - (int)pos.y - 1,
            1,
            1,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            pixel.data()
        );
    }
}
