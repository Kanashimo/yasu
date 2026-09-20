#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <memory>

#include "Module.h"
#include "ScreenCapture/ScreenCapture.h"

struct Instance {
    GLFWwindow *window;
    ImGuiContext *context;
    const Monitor *monitor;
};

class Application
{
    public:
        Application();
        ~Application();

        void run();
        void close();
        Instance *get_active_instance();

        template <typename T_Module>
        requires(std::is_base_of_v<Module, T_Module>)
        void attach()
        {
            modules.push_back(std::make_unique<T_Module>());
        }

        template<typename T_Module>
        requires(std::is_base_of_v<Module, T_Module>)
        T_Module *module()
        {
            for (const auto &module : modules) {
                if (auto casted = dynamic_cast<T_Module*>(module.get()))
                {
                    return casted;
                }
            }
            return nullptr;
        }

        static Application &application();

    private:
        ScreenCapture screenCapture;
        const std::vector<Monitor> &monitors;
        std::vector<Instance> instances;
        std::vector<std::unique_ptr<Module>> modules;

        Instance *active_instance = nullptr;
        bool initialized = false;
        bool should_close = false;

        void init_instance(Instance &instance);

        friend class Module;

};
