#pragma once

#include <imgui.h>

#include "src/Application.h"
#include "src/Module.h"

namespace UI {

    class Exit : public Module
    {
        public:
            void on_after_render(Instance &) override
            {
                if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && !exit_disabled)
                {
                    Application::application().close();
                }
                exit_disabled = false;
            }

            void disable_exit()
            {
                exit_disabled = true;
            }

        private:
            bool exit_disabled = false;
    };

}
