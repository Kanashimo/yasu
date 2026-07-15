#include <gtk/gtk.h>
#include <libportal/portal.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <vector>
#include <stdexcept>
#include <string>

#include "ScreenCapture.h"
#include "ScreenCaptureError.h"


ScreenCapture::ScreenCapture() : portal(nullptr), loop(nullptr), error(nullptr), cancellable(nullptr)
{
    gtk_init();
    portal = xdp_portal_new();

    if (!portal)
    {
        throw std::runtime_error("Failed to create org.freedesktop.portal");
    }

    loop = g_main_loop_new(nullptr, false);
    cancellable = g_cancellable_new();

    if (!loop)
    {
        g_object_unref(portal);
        throw std::runtime_error("Failed to create GMainLoop");
    }

    xdp_portal_take_screenshot(
        portal,
        NULL,
        XDP_SCREENSHOT_FLAG_NONE,
        cancellable,
        callback,
        this
    );

    g_main_loop_run(loop);
    g_main_loop_unref(loop);
    g_object_unref(portal);

    if (cancellable)
    {
        g_object_unref(cancellable);
    }

    if (error)
    {

        if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_FAILED))
        {
            g_error_free(error);
            error = nullptr;
            g_set_error(&error, SCREENCAPTURE_ERROR, SCREENCAPTURE_ERROR_SCREENSHOT_FAILED, "Screenshot failed, check application permissions");
        }

        std::string domain = g_quark_to_string(error->domain);
        std::string message = error->message;
        int code = error->code;
        std::string error_message = message + " [" + domain + + ", " + std::to_string(code) + "]";

        g_error_free(error);
        throw std::runtime_error(error_message);
    }

    printf("🤨📸 caught in 4k\n");
}

ScreenCapture::~ScreenCapture()
{
    for(Monitor &monitor : _monitors)
    {
        g_free(monitor.framebuffer);
        monitor.framebuffer = nullptr;
    }
}

const std::vector<Monitor> *ScreenCapture::monitors() const
{
    return &_monitors;
}

const Monitor *ScreenCapture::get_monitor_from_pos(int x, int y) const
{
    for (const Monitor &monitor : _monitors)
    {
        if (monitor.x == x && monitor.y == y)
        {
            return &monitor;
        }
    }
    return nullptr;
}

void ScreenCapture::callback(GObject *object, GAsyncResult *result, gpointer data)
{
    ScreenCapture *self = static_cast<ScreenCapture*>(data);
    self->take_screenshot(object, result);
}

void ScreenCapture::take_screenshot(GObject *object, GAsyncResult *result)
{
    g_autofree char *uri = xdp_portal_take_screenshot_finish(portal, result, &error);

    if (error) {
        g_main_loop_quit(loop);
        return;
    }

    g_autoptr(GFile) file = g_file_new_for_uri(uri);
    g_autofree char *path = g_file_get_path(file);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(path, &error);

    if (error) {
        if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        {
            g_error_free(error);
            error = nullptr;
        }
        g_file_delete(file, NULL, NULL);
        g_main_loop_quit(loop);
        return;
    }

    g_file_delete(file, NULL, &error);

    if (error) {
        g_main_loop_quit(loop);
        g_object_unref(pixbuf);
        return;
    }

    this->process_screenshot(pixbuf);
}

void ScreenCapture::process_screenshot(GdkPixbuf *pixbuf)
{
    GdkDisplay *display = gdk_display_get_default();

    if (!display)
    {
        g_set_error(&error, SCREENCAPTURE_ERROR, SCREENCAPTURE_ERROR_DISPLAY_SERVER_UNAVAILABLE, "Failed to fetch display server");
        g_object_unref(pixbuf);
        g_main_loop_quit(loop);
        return;
    }

    GListModel *monitors = gdk_display_get_monitors(display);

    if(!monitors)
    {
        g_set_error(&error, SCREENCAPTURE_ERROR, SCREENCAPTURE_ERROR_MONITORS_UNAVAILABLE, "Failed to fetch monitors");
        g_object_unref(pixbuf);
        g_main_loop_quit(loop);
        return;
    }

    guint n_monitors = g_list_model_get_n_items(monitors);

    if (n_monitors == 0)
    {
        g_set_error(&error, SCREENCAPTURE_ERROR, SCREENCAPTURE_ERROR_MONITOR_NOT_FOUND, "Failed to get monitor, is there any connected?");
        g_object_unref(pixbuf);
        g_main_loop_quit(loop);
        return;
    }

    // width and height of all monitors merged together
    int screen_width = gdk_pixbuf_get_width(pixbuf);
    int screen_height = gdk_pixbuf_get_height(pixbuf);

    for (guint i = 0; i < n_monitors; i++)
    {
        Monitor monitor;
        GdkMonitor *gdk_monitor = GDK_MONITOR(g_list_model_get_item(monitors, i));
        GdkRectangle rectangle;

        gdk_monitor_get_geometry(gdk_monitor, &rectangle);
        const char *connector = gdk_monitor_get_connector(gdk_monitor);

        if (rectangle.x + rectangle.width <= screen_width && rectangle.y + rectangle.height <= screen_height)
        {
            GdkPixbuf *monitor_pixbuf = gdk_pixbuf_new_subpixbuf(
                pixbuf,
                rectangle.x,
                rectangle.y,
                rectangle.width,
                rectangle.height
            );

            guchar *framebuffer = gdk_pixbuf_get_pixels(monitor_pixbuf);
            gsize framebuffer_size = gdk_pixbuf_get_byte_length(monitor_pixbuf);

            monitor.id = i + 1;
            monitor.connector = connector;
            monitor.framebuffer = static_cast<guchar *>(g_memdup2(framebuffer, framebuffer_size));
            monitor.width = rectangle.width;
            monitor.height = rectangle.height;
            monitor.x = rectangle.x;
            monitor.y = rectangle.y;

            g_object_unref(monitor_pixbuf);
            _monitors.push_back(std::move(monitor));
        } else {
            g_set_error(&error, SCREENCAPTURE_ERROR, SCREENCAPTURE_ERROR_OUT_OF_BOUNDS, "Pixel buffer out of bounds for %s (%d)", connector, i + 1);
            g_object_unref(gdk_monitor);
            g_object_unref(pixbuf);
            g_main_loop_quit(loop);
            return;
        }

        g_object_unref(gdk_monitor);

    }

    g_object_unref(pixbuf);
    g_main_loop_quit(loop);
}
