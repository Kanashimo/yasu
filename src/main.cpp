#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <vector>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "ScreenCapture/ScreenCapture.h"
#include "ShaderLoader/ShaderLoader.h"

void process_drag(Monitor monitor, GLFWwindow *window, GLuint texture, ImVec2 &export_start_pos, ImVec2 &export_end_pos, bool &export_dragging, float &export_drag_begin_time)
{
    static ImVec2 start_pos;
    static ImVec2 end_pos;
    static bool dragging = false;
    static float drag_begin_time = 0;
    static GLFWwindow *targeted_window = nullptr;
    ImVec2 pos = ImGui::GetMousePos();

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        start_pos = pos;
        dragging = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        targeted_window = window;
        drag_begin_time = glfwGetTime();
    }

    if (dragging && targeted_window == window)
    {

        ImVec2 r_min(
            std::min(start_pos.x, pos.x),
            std::min(start_pos.y, pos.y)
        );

        ImVec2 r_max(
            std::max(start_pos.x, pos.x),
            std::max(start_pos.y, pos.y)
        );

        float u0 = r_min.x / (float)monitor.width;
        float u1 = r_max.x / (float)monitor.width;

        float v0 = r_min.y / (float)monitor.height;
        float v1 = r_max.y / (float)monitor.height;

        ImGui::GetForegroundDrawList()->AddImage(
            (ImTextureID)(intptr_t)texture,
            r_min,
            r_max,
            ImVec2(u0, v0),
            ImVec2(u1, v1)
        );

        ImGui::GetForegroundDrawList()->AddRect(
            start_pos,
            pos,
            IM_COL32(90, 200, 255, 200),
            0.0f,
            0,
            2.0f
        );

        export_drag_begin_time = drag_begin_time;
        export_start_pos = start_pos;
        export_end_pos = pos;
        end_pos = pos;
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        dragging = false;
        targeted_window = nullptr;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;


        // int width  = std::abs((int)end_pos.x - (int)start_pos.x) + 1;
        // int height = std::abs((int)end_pos.y - (int)start_pos.y) + 1;
        // printf("%d %d\n", width, height);
        // printf("ID; %d\n%.2f, %.2f\n%.2f, %.2f\n", monitor.id, start_pos.x ,start_pos.y, end_pos.x+1, end_pos.y+1);
    }
    export_dragging = dragging;
}

void process_exit(bool &close)
{
    if (ImGui::IsKeyDown(ImGuiKey_Escape))
    {
        close = true;
        printf("Exited gracefuly\n");
    }
}

void show_debug()
{
    static bool show = false;
    if (ImGui::IsKeyPressed(ImGuiKey_F1))
    {
        show = !show;
    }
    if (show)
    {
        float fps = ImGui::GetIO().Framerate;
        float frametime = 1000 / fps;
        ImGui::Begin("debug");
        ImGui::Text("fps: %.1f", fps);
        ImGui::Text("frametime: %.1f", frametime);
        ImGui::End();
    }
}


int main()
{
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

        GLFWmonitor *glfw_monitor = glfw_monitors[i];

        int x, y;
        glfwGetMonitorPos(glfw_monitor, &x, &y);
        const Monitor *monitor = screenCapture.get_monitor_from_pos(x, y);

        if (!monitor)
        {
            std::cerr << "Failed to assign GLFW window to monitor" << std::endl;
            return -3;
        }

        // std::cout << i + 1 << ": " << x << ", " << y << ", " << monitor->connector << std::endl;

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
            std::cerr << "Failed to create GLFW window" << std::endl;
            return -4;
        }

        glfwMakeContextCurrent(window);

        if (!ShaderLoader::init())
        {
            std::cerr << ShaderLoader::glew_get_error() << std::endl;
            glfwTerminate();
            return -69;
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        windows.push_back(window);
    }

    bool close = false;
    bool initialized = false;

    std::vector<ImGuiContext *> windowContext;
    std::vector<GLuint> texture(windows.size());
    std::vector<std::unique_ptr<ShaderLoader>> program(windows.size());
    std::vector<GLuint> arrays(windows.size());

    while (!close) {

        glfwPollEvents();

        for (size_t i = 0; i < windows.size(); i++)
        {

            GLFWwindow *window = windows[i];
            const Monitor &monitor = monitors[i];

            glfwMakeContextCurrent(window);

            if (!initialized)
            {
                glfwSwapInterval(0);
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

                GLuint array;
                glGenVertexArrays(1, &array);

                try {
                    auto loader = std::make_unique<ShaderLoader>();
                    loader->load(GL_VERTEX_SHADER, {
                        "vert.glsl"
                    });
                    loader->load(GL_FRAGMENT_SHADER, {
                        "frag.glsl"
                    });
                    // TODO: fix segfault on comp err
                    loader->compile();
                    loader->use();
                    program[i] = std::move(loader);
                    arrays[i] = array;
                } catch(std::runtime_error &e) {
                    std::cerr << e.what() << std::endl;
                }

                glUniform2f(glGetUniformLocation(program[i]->get(), "uScreenResolution"), (float)monitor.width, (float)monitor.height);
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

            static double begin_time = glfwGetTime();
            double current_time = glfwGetTime() - begin_time;

            ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);

            ImGui::GetBackgroundDrawList()->AddImage(
                (ImTextureID)(intptr_t)texture[i],
                ImVec2(0, 0),
                ImVec2((float)monitor.width, (float)monitor.height)
            );

            float fade = (float)(current_time / 0.70f);
            fade = std::clamp(fade, 0.0f, 0.40f);

            ImGui::GetBackgroundDrawList()->AddRectFilled(
                ImVec2(0, 0),
                ImVec2((float)monitor.width, (float)monitor.height),
                IM_COL32(0, 0, 0, (int)(fade * 255.0f))
            );

            // ImGui::Begin("Hello World");
            // ImGui::Text("Hello, World!");
            // ImGui::End();

            ImVec2 start_pos, end_pos;
            float drag_begin_time = 0;
            bool dragging;

            process_drag(monitor, window, texture[i], start_pos, end_pos, dragging, drag_begin_time);

            process_exit(close);

            show_debug();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            float strength = 0;
            if (dragging)
            {
                // strength = (float)((glfwGetTime() - drag_begin_time) / 0.23f);
                strength = (float)((glfwGetTime() - drag_begin_time) / 0.30f);
                strength = std::clamp(strength, 0.0f, 1.0f);
                strength = 1.0f - std::pow(1.0f - strength, 3.0f);
            }

            if (strength != 0)
            {
                std::cout << strength << std::endl;
            }


            // BEGIN OPENGL
            program[i]->use();
            GLuint &array = arrays[i];
            glBindVertexArray(array);
            // glUniform2f(glGetUniformLocation(program[i]->get(), "uStartPoint"), 0.0f, 0.0f);
            // glUniform2f(glGetUniformLocation(program[i]->get(), "uEndPoint"), 1920.0f, 1080.0f);
            glUniform2f(glGetUniformLocation(program[i]->get(), "uStartPoint"), start_pos.x, start_pos.y);
            glUniform2f(glGetUniformLocation(program[i]->get(), "uEndPoint"), end_pos.x, end_pos.y);
            glUniform1f(glGetUniformLocation(program[i]->get(), "uStrength"), strength);
            if (dragging)
            {
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            glBindVertexArray(0);
            // END OPENGL

            glfwSwapBuffers(window);

        }

        initialized = true;

    }

    for (size_t i = 0; i < windows.size(); i++)
    {
        // TODO: verify that it doesn't leak or something else
        glfwMakeContextCurrent(windows[i]);
        ImGui::SetCurrentContext(windowContext[i]);
        ImGui_ImplGlfw_Shutdown();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext(windowContext[i]);
        glfwDestroyWindow(windows[i]);
    }

    program.clear();
    windowContext.clear();
    windows.clear();
    glfwTerminate();

    return 0;
}
