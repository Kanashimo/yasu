#include <memory>
#include <stdexcept>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Application.h"
#include "Module.h"
#include "ScreenCapture/ScreenCapture.h"
#include "ShaderLoader/ShaderLoader.h"

static Application *App = nullptr;

Application::Application() : monitors(screenCapture.monitors())
{
    glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_WAYLAND_DISABLE_LIBDECOR);
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    int n_glfw_monitors = 0;
    GLFWmonitor **glfw_monitors = glfwGetMonitors(&n_glfw_monitors);

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

    for (int i = 0; i < n_glfw_monitors; i++)
    {
        Instance instance;

        App = this;

        int x, y;
        GLFWmonitor *glfw_monitor = glfw_monitors[i];

        glfwGetMonitorPos(glfw_monitor, &x, &y);
        const Monitor *monitor = screenCapture.get_monitor_from_pos(x, y);

        if (!monitor)
        {
            throw std::runtime_error("Failed to assign GLFW window to monitor");
        }

        const GLFWvidmode *mode = glfwGetVideoMode(glfw_monitor);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        GLFWwindow *window = glfwCreateWindow(
            monitor->width,
            monitor->height,
            "yasu",
            glfw_monitor,
            nullptr
        );

        if (!window)
        {
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);

        if (!ShaderLoader::init())
        {
            throw std::runtime_error(ShaderLoader::glew_get_error());
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        instance.window = window;
        instance.monitor = monitor;
        instances.push_back(std::move(instance));
    }
}

void Application::run()
{
    while (!should_close) {

        glfwPollEvents();

        if (!initialized)
        {
            for (const std::unique_ptr<Module> &module : modules)
            {
                module->on_init();
            }
        }

        for (auto &instance: instances)
        {
            active_instance = &instance;

            GLFWwindow *window = instance.window;

            glfwMakeContextCurrent(window);

            if (!initialized) init_instance(instance);

            if (glfwWindowShouldClose(window))
            {
                close();
                break;
            }

            ImGui::SetCurrentContext(instance.context);

            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);

            for (const std::unique_ptr<Module> &module : modules)
            {
                module->on_update(instance);
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            for (const std::unique_ptr<Module> &module : modules)
            {
                module->on_after_render(instance);
            }

            glfwSwapBuffers(window);
        }

        initialized = true;

    }
}

void Application::close()
{
    should_close = true;

    for (const std::unique_ptr<Module> &module : modules)
    {
        module->on_close();
    }
}

Instance *Application::get_active_instance()
{
    return active_instance;
}

void Application::init_instance(Instance &instance)
{
    glfwSwapInterval(0);
    ImGuiContext *context = ImGui::CreateContext();
    ImGui::SetCurrentContext(context);
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::GetIO().LogFilename = nullptr;
    instance.context = context;

    ImGui_ImplGlfw_InitForOpenGL(instance.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    for (const std::unique_ptr<Module> &module : modules)
    {
        module->on_instance_init(instance);
    }
}

Application &Application::application()
{
    assert(App);
    return *App;
}

Application::~Application()
{
    // TODO: verify that it doesn't leak or something else
    // TODO: check if window and/or context is not nullptr

    for (auto &instance : instances) {
        // glDeleteTextures(1, &instance.texture);
        glfwMakeContextCurrent(instance.window);
        ImGui::SetCurrentContext(instance.context);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(instance.context);
        glfwDestroyWindow(instance.window);
    }

    instances.clear();
    glfwTerminate();
    App = nullptr;
}
