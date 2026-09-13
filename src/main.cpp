#include <GL/glew.h>
#include <GL/gl.h>
#include <algorithm>
#include <exception>
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

// void load_shaders(bool &close, GLuint &program, GLuint &VAO, GLuint &VBO)
// {
//     try {
//         ShaderSource box_src = ShaderLoader::load("box.glsl");
//         ShaderSource glow_src = ShaderLoader::load("glow.glsl");
//         Shader box = ShaderLoader::compile(GL_VERTEX_SHADER, box_src);
//         Shader glow = ShaderLoader::compile(GL_FRAGMENT_SHADER, glow_src);
//         ShaderLoader::attach(program, box);
//         ShaderLoader::attach(program, glow);
//         ShaderLoader::link(program);
//         glDeleteShader(box);
//         glDeleteShader(glow);
//         glUseProgram(program);
//         float vertices[] = {
//             -0.5f, -0.5f, 0.0f,
//              0.5f, -0.5f, 0.0f,
//              0.0f,  0.5f, 0.0f
//         };
//         glGenVertexArrays(1, &VAO);
//         glGenBuffers(1, &VBO);
//         glBindVertexArray(VAO);
//         glBindBuffer(GL_ARRAY_BUFFER, VBO);
//         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//         glEnableVertexAttribArray(0); // Włączamy atrybut o indeksie 0
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//         glBindVertexArray(0);
//     } catch (std::runtime_error &e) {
//         close = true;
//         std::cerr << e.what() << std::endl;
//     }
// }

void process_drag(Monitor monitor, GLFWwindow *window, GLuint texture)
{
    static ImVec2 start_pos;
    static ImVec2 end_pos;
    static bool dragging = false;
    static GLFWwindow *targeted_window = nullptr;
    ImVec2 pos = ImGui::GetMousePos();

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        start_pos = pos;
        dragging = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        targeted_window = window;
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
    std::vector<GLuint> arrays(windows.size()); // VAO
    std::vector<GLuint> buffers(windows.size()); // VBO

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

                float points[6] = {
                    -0.25f, -0.25f,
                    0.f, 0.35f,
                    0.25f, -0.25f
                };

                // VAO
                GLuint array;
                // VBO
                GLuint buffer;

                glGenVertexArrays(1, &array);
                glBindVertexArray(array);
                glGenBuffers(1, &buffer);
                glBindBuffer(GL_ARRAY_BUFFER, buffer);
                glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), points, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
                glBindVertexArray(0);

                try {
                    auto loader = std::make_unique<ShaderLoader>();
                    loader->load(GL_VERTEX_SHADER, {
                        "box.glsl"
                    });
                    loader->load(GL_FRAGMENT_SHADER, {
                        "glow.glsl"
                    });
                    loader->compile();
                    loader->use();
                    // std::cout << array << buffer << std::endl;
                    program[i] = std::move(loader);
                    buffers[i] = buffer;
                    arrays[i] = array;
                } catch(std::runtime_error &e) {
                    std::cerr << e.what() << std::endl;
                }

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

            process_drag(monitor, window, texture[i]);

            process_exit(close);

            show_debug();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // BEGIN OPENGL
            program[i]->use();
            GLuint &array = arrays[i];
            glBindVertexArray(array);
            glDrawArrays(GL_TRIANGLES, 0, 3);
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

    windowContext.clear();
    windows.clear();
    glfwTerminate();

    return 0;
}
