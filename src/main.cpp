#include "ScreenCapture/ScreenCapture.h"
#include <cstdio>
#include <iostream>
#include <glib.h>
#include <gio/gio.h>
#include <libportal/portal.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdexcept>
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
    catch (std::runtime_error e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
