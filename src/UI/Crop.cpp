#include <imgui.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <stdexcept>
#include "Toolbar.h"

#include "src/Application.h"
#include "src/ShaderLoader/ShaderLoader.h"
#include "src/UI/Crop.h"
#include "src/UI/BackgroundTexture.h"
#include "src/UI/Exit.h"


namespace UI {

    void Crop::on_instance_init(Instance &instance)
    {
        gl::GLuint array;
        gl::glGenVertexArrays(1, &array);
        auto loader = std::make_unique<ShaderLoader>();
        loader->load(gl::GL_VERTEX_SHADER, {
            "vert.glsl",
        });
        loader->load(gl::GL_FRAGMENT_SHADER, {
            "frag.glsl"
        });
        // TODO: fix segfault on comp err
        loader->compile();
        loader->use();

        gl::glUniform2f(
            gl::glGetUniformLocation(loader->get(), "uScreenResolution"),
            (float)instance.monitor->width,
            (float)instance.monitor->height
        );

        programs[&instance] = std::move(loader);
        arrays[&instance] = array;
    }

    void Crop::on_update(Instance &instance)
    {
        if (Application::application().module<Toolbar>()->get_current_option() != OPTION_CROP) return;

        pos = ImGui::GetMousePos();

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !dragging && !ImGui::GetIO().WantCaptureMouse)
        {
            start_pos = pos;
            dragging = true;
            glfwSetInputMode(instance.window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
            targeted_window = instance.window;
            // TODO: debloat this shit cuz it's useless, unnecessary pointers
            texture = Application::application().module<BackgroundTexture>()->get_texture(instance);
            begin_time = glfwGetTime();
            strength = 0.0f;
            Application::application().module<Toolbar>()->set_busy(true);
        }

        if (ImGui::IsKeyDown(ImGuiKey_Escape) && dragging)
        {
            Application::application().module<Exit>()->disable_exit();
            Application::application().module<Toolbar>()->set_busy(false);
            stop_dragging(instance);
        }

        if (dragging && targeted_window == instance.window)
        {
            ImVec2 r_min(
                std::min(start_pos.x, pos.x),
                std::min(start_pos.y, pos.y)
            );

            ImVec2 r_max(
                std::max(start_pos.x, pos.x),
                std::max(start_pos.y, pos.y)
            );

            float u0 = r_min.x / instance.monitor->width;
            float u1 = r_max.x / instance.monitor->width;

            float v0 = r_min.y / instance.monitor->height;
            float v1 = r_max.y / instance.monitor->height;

            ImGui::GetForegroundDrawList()->AddImage(
                (ImTextureID)(intptr_t)*texture,
                r_min,
                r_max,
                ImVec2(u0, v0),
                ImVec2(u1, v1)
            );

            ImGui::GetForegroundDrawList()->AddRect(
                start_pos,
                pos,
                IM_COL32(90, 200, 255, 200),
                0.0f,
                0,
                // 2.5f
                2.0f
            );

            strength = (float)((glfwGetTime() - begin_time) / 0.50f);
            strength = std::clamp(strength, 0.0f, 1.0f);
            strength = 1.0f - std::pow(1.0f - strength, 3.0f);
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            stop_dragging(instance);
        }
    }

    void Crop::on_after_render(Instance &instance)
    {
        if (!dragging || targeted_window != instance.window) return;

        if (!programs.contains(&instance) || !arrays.contains(&instance))
        {
            throw std::runtime_error("Cannot find program and/or array (VAO) for active instance");
        }

        programs[&instance]->use();
        gl::GLuint &array = arrays[&instance];

        gl::glBindVertexArray(array);

        gl::glUniform2f(
          gl::glGetUniformLocation(programs[&instance]->get(), "uStartPoint"),
          start_pos.x,
          start_pos.y
        );

        gl::glUniform2f(
          gl::glGetUniformLocation(programs[&instance]->get(), "uEndPoint"),
          pos.x,
          pos.y
        );

        gl::glUniform1f(
            gl::glGetUniformLocation(programs[&instance]->get(), "uStrength"),
            strength
        );

        gl::glDrawArrays(gl::GL_TRIANGLES, 0, 6);
        gl::glBindVertexArray(0);
    }

    Crop::~Crop()
    {
        programs.clear();
    }

    void Crop::stop_dragging(Instance &instance)
    {
        dragging = false;
        targeted_window = nullptr;
        texture = nullptr;
        glfwSetInputMode(instance.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
        begin_time = 0.0;
        strength = 0.0f;
    }

}
