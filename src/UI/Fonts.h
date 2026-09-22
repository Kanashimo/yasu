#pragma once

#include "src/Application.h"
#include "src/Module.h"
#include "imgui.h"
#include <stdexcept>

#include "Roboto_Regular.h"
#include "Icons.h"



#define ICON_RULER "\ue800"
#define ICON_SETTINGS "\ue801"
#define ICON_PIPETTE "\ue802"
#define ICON_CROP "\ue803"

namespace UI {

    class Fonts : public Module
    {
        public:
            void on_instance_init(Instance &instance) override
            {
                ImFontConfig roboto_cfg;
                roboto_cfg.FontDataOwnedByAtlas = false;

                ImFont *font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
                    (void*)Font::Roboto_Regular,
                    Font::Roboto_Regular_Size,
                    24.0f,
                    &roboto_cfg
                );

                ImFontConfig icons_cfg;
                icons_cfg.FontDataOwnedByAtlas = false;
                icons_cfg.MergeMode = true;
                icons_cfg.PixelSnapH = true;

                static const ImWchar icons_ranges[] = { 0x0020, 0xFFFF, 0 };

                ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
                    (void*)Font::Icons,
                    Font::Icons_Size,
                    24.0f,
                    &icons_cfg,
                    icons_ranges
                );

                if (font == nullptr)
                {
                    throw std::runtime_error("Failed to load fonts");
                }

                ImGui::GetIO().Fonts->Build();
            }
    };

}
