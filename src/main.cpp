#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>

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

    while (!close) {

        for (size_t i = 0; i < windows.size(); i++)
        {

            GLFWwindow* window = windows[i];

            if (glfwWindowShouldClose(window))
            {
                close = true;
                break;
            }

            glfwMakeContextCurrent(window);
            glfwSwapBuffers(window);
        }

        glfwPollEvents();
    }


    for (GLFWwindow *window : windows)
    {
        glfwDestroyWindow(window);
    }

    windows.clear();

    return 0;
}
