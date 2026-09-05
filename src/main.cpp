#include <GL/gl.h>
#include <iostream>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include "ScreenCapture/ScreenCapture.h"


int main()
{
    // try {
    //     ScreenCapture screenCapture;
    //     std::vector<Monitor> monitors = screenCapture.monitors();
    //     for (Monitor monitor : monitors)
    //     {
    //         printf("\nmonitor: %d\nx: %d\ny: %d\nw: %d\nh: %d\nc: %s\n\n", monitor.id, monitor.x, monitor.y, monitor.width, monitor.height, monitor.connector.c_str());
    //     }
    // }
    // catch (std::runtime_error &e) {
    //     std::cerr << e.what() << std::endl;
    //     return -1;
    // }

    ScreenCapture screenCapture;
    const std::vector<Monitor> &monitors = screenCapture.monitors();

    glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_WAYLAND_DISABLE_LIBDECOR);
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -2;
    }

    int n_glfw_monitors = 0;
    GLFWmonitor **glfw_monitors = glfwGetMonitors(&n_glfw_monitors);

    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

    std::vector<GLFWwindow*> windows;

    for (int i = 0; i < n_glfw_monitors; i++)
    {
        int x, y;
        glfwGetMonitorPos(glfw_monitors[i], &x, &y);
        const Monitor *monitor = screenCapture.get_monitor_from_pos(x, y);

        if (!monitor)
        {
            std::cerr << "Failed to assign GLFW window to monitor" << std::endl;
            return -3;
        }

        // std::cout << i + 1 << ": " << x << ", " << y << ", " << monitor->connector << std::endl;

        GLFWwindow *window = glfwCreateWindow(
            monitor->width,
            monitor->height,
            "yasu",
            glfw_monitors[i],
            nullptr
        );

        // glfwSetWindowMonitor(window, glfw_monitors[i], 0, 0, monitor->width, monitor->height, GLFW_DONT_CARE);

        if (!window)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            return -4;
        }

        windows.push_back(window);
    }

    bool close = false;
    bool initialized = false;

    std::vector<ImGuiContext *> windowContext;
    std::vector<GLuint> texture(windows.size());

    while (!close) {

        glfwPollEvents();

        for (size_t i = 0; i < windows.size(); i++)
        {

            GLFWwindow *window = windows[i];
            const Monitor &monitor = monitors[i];

            glfwMakeContextCurrent(window);

            if (!initialized)
            {
                ImGuiContext *context = ImGui::CreateContext();
                ImGui::SetCurrentContext(context);
                ImGui::GetIO().IniFilename = nullptr;
                ImGui::GetIO().LogFilename = nullptr;
                windowContext.push_back(context);
                ImGui_ImplGlfw_InitForOpenGL(window, true);
                ImGui_ImplOpenGL3_Init("#version 330");

                glGenTextures(1, &texture[i]);
                glBindTexture(GL_TEXTURE_2D, texture[i]);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glBindTexture(GL_TEXTURE_2D, texture[i]);

                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
                glPixelStorei(GL_UNPACK_ROW_LENGTH, monitor.stride / monitor.channels);

                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    monitor.alpha ? GL_RGBA8 : GL_RGB8,
                    monitor.width,
                    monitor.height,
                    0,
                    monitor.alpha ? GL_RGBA : GL_RGB,
                    GL_UNSIGNED_BYTE,
                    monitor.framebuffer);

                glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            }

            ImGui::SetCurrentContext(windowContext[i]);

            if (glfwWindowShouldClose(window))
            {
                close = true;
                break;
            }

            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();


            ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);

            ImGui::Begin("Hello World");
            ImGui::Text("Hello, World!");
            ImGui::End();


            ImGui::Begin("Screenshot");
            ImGui::Image(
                (ImTextureID)(intptr_t)texture[i],
                ImVec2((float)monitor.width, (float)monitor.height));
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);

        }

        initialized = true;

    }

    for (size_t i = 0; i < windows.size(); i++)
    {
        glfwMakeContextCurrent(windows[i]);
        ImGui::SetCurrentContext(windowContext[i]);
        ImGui_ImplGlfw_Shutdown();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext(windowContext[i]);
        glfwDestroyWindow(windows[i]);
    }

    windowContext.clear();
    windows.clear();
    glfwTerminate();

    return 0;
}
