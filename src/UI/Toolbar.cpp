#include "Toolbar.h"
#include "imgui.h"
#include "src/Application.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include "algorithm"
#include "Fonts.h"

namespace UI {

    void Toolbar::on_instance_init(Instance &instance)
    {
        state[&instance] = ToolbarState{};
    }

    void Toolbar::on_update(Instance &instance)
    {
        ImVec2 size = ImGui::GetIO().DisplaySize;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(90, 200, 255, 255));

        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

        int active = glfwGetWindowAttrib(instance.window, GLFW_HOVERED);

        float target = active ? 1.0f : 0.0f;
        if (busy) target = 0.0f;

        float speed = 20.0f;

        state[&instance].deflection +=
            (target - state[&instance].deflection) *
            speed *
            ImGui::GetIO().DeltaTime;

        state[&instance].deflection = std::clamp(
            state[&instance].deflection,
            0.0f,
            1.0f
        );

        float t = state[&instance].deflection;
        t = 1.0f - std::pow(1.0f - t, 3.0f);

        ImGui::SetNextWindowPos(
          ImVec2(size.x * 0.5f, 40.0f * (state[&instance].deflection * 2 - 1)),
          ImGuiCond_Always,
          ImVec2(0.5f, 0.5f)
        );

        ImGui::Begin("Toolbar", nullptr, window_flags);

        render_button(OPTION_CROP, ICON_CROP, "Crop");
        ImGui::SameLine();
        render_button(OPTION_RULER, ICON_RULER, "Ruler");
        ImGui::SameLine();
        render_button(OPTION_PIPETTE, ICON_PIPETTE, "Pipette");
        ImGui::SameLine();
        render_button(OPTION_SETTINGS, ICON_SETTINGS, "Settings");

        ImGui::End();

        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        if (current_window != prev_window)
        {
            std::cout << current_window << std::endl;
        }

        prev_window = current_window;
    }

    ToolbarOption Toolbar::get_current_option()
    {
        return current_option;
    }

    void Toolbar::set_busy(bool state)
    {
        busy = state;
        std::cout << "busy" << std::endl;
    }

    void Toolbar::render_button(ToolbarOption option, const char* icon, const char* tooltip_label)
    {
        ToolbarOption current = current_option;

        if (current == option)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(46, 46, 46, 255));
        }

        if (ImGui::Button(icon, ImVec2(48, 48)))
        {
            current_option = option;
        }

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            if (ImGui::BeginTooltip())
            {
                ImGui::TextUnformatted(tooltip_label);
                ImGui::EndTooltip();
            }
        }

        if (current == option)
        {
            ImGui::PopStyleColor();
        }
    }
}
