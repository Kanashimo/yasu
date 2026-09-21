#pragma once

#include "src/Module.h"
#include "imgui.h"

#include "Roboto_Regular.h"
#include <stdexcept>

namespace UI {

    class Init : public Module
    {
        public:
            void on_instance_init(Instance &) override
            {
                ImFontConfig font_cfg;
                font_cfg.FontDataOwnedByAtlas = false;

                ImFont* font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
                    (void*)Font::Roboto_Regular,
                    Font::Roboto_Regular_Size,
                    24.0f,
                    &font_cfg
                );

                if (font == nullptr)
                {
                    throw std::runtime_error("Failed to load fonts");
                }

                ImGui::GetIO().Fonts->Build();
            }
    };

}
