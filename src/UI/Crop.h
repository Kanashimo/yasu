#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <memory>
#include <unordered_map>

#include "src/Module.h"
#include "src/ShaderLoader/ShaderLoader.h"

namespace UI {

    class Crop : public Module
    {
        public:
            void on_instance_init(Instance &instance) override;
            void on_update(Instance &instance) override;
            void on_after_render(Instance &instance) override;

            ~Crop();

        private:
            ImVec2 start_pos;
            ImVec2 end_pos;
            ImVec2 pos;

            bool dragging;
            GLFWwindow* targeted_window;
            gl::GLuint *texture;

            std::unordered_map<Instance*, gl::GLuint> arrays;
            std::unordered_map<Instance*, std::unique_ptr<ShaderLoader>> programs;

            double begin_time;
            float strength;

            void stop_dragging(Instance &instance);
    };

}
