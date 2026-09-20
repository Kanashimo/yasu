#pragma once

#include <imgui.h>

#include "src/Module.h"

namespace UI {

    class Debug : public Module
    {
        private:
            bool show = false;

        public:
            void on_update(Instance &) override
            {
                if (ImGui::IsKeyPressed(ImGuiKey_F1))
                {
                    show = !show;
                }

                if (show)
                {
                    float fps = ImGui::GetIO().Framerate;
                    float frametime = 1000.0f / fps;

                    ImGui::Begin("debug");
                    ImGui::Text("fps: %.1f", fps);
                    ImGui::Text("frametime: %.1f", frametime);
                    ImGui::End();
                }
            }
    };

}
