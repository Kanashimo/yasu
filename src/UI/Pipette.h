#pragma once

#include <vector>
#include <imgui.h>

#include "src/Module.h"

namespace UI {

    class Pipette : public Module
    {
        public:
            void on_update(Instance &instance) override;
            void on_after_render(Instance &instance) override;

        private:
            ImVec2 pos;
            std::vector<unsigned char> pixel;
            bool dragging = false;
    };

}
