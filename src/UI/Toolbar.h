#include <GLFW/glfw3.h>
#include <unordered_map>

#include "src/Module.h"

namespace UI {

    struct ToolbarState {
        float deflection = 0.0f;
        double begin_time = 0.0;
    };

    enum ToolbarOption {
        OPTION_CROP,
        OPTION_RULER,
        OPTION_PIPETTE,
        OPTION_SETTINGS
    };

    class Toolbar : public Module
    {
        public:
            void on_instance_init(Instance &instance) override;
            void on_update(Instance &instance) override;
            ToolbarOption get_current_option();
            void set_busy(bool state);

        private:
            ToolbarOption current_option = OPTION_CROP;
            GLFWwindow* prev_window;
            GLFWwindow* current_window;
            std::unordered_map<Instance*, ToolbarState> state;
            bool busy = false;
            void render_button(ToolbarOption option, const char* icon, const char* tooltip_label);
    };

}
