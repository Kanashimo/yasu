#include "ScreenCapture/ScreenCapture.h"
#include <iostream>
#include <vector>


int main()
{
    try {
        ScreenCapture screenCapture;
        std::vector<Monitor> monitors = screenCapture.monitors();
        for (Monitor monitor : monitors)
        {
            printf("\nmonitor: %d\nx: %d\ny: %d\nw: %d\nh: %d\nc: %s\n\n", monitor.id, monitor.x, monitor.y, monitor.width, monitor.height, monitor.connector.c_str());
        }
    }
    catch (std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
