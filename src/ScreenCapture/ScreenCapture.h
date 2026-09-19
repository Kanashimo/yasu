#pragma once

#include <gtk/gtk.h>
#include <libportal/portal.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <vector>
#include <string>


struct Monitor
{
    unsigned int id;
    std::string connector;
    guchar *framebuffer;
    int stride;
    int channels;
    bool alpha;
    int width;
    int height;
    int x;
    int y;

};

class ScreenCapture
{
    public:

        ScreenCapture();
        ~ScreenCapture();

        const std::vector<Monitor> &monitors() const;
        const Monitor *get_monitor_from_pos(int x, int y) const;

    private:

        XdpPortal *portal = nullptr;
        GMainLoop *loop = nullptr;
        GError *error = nullptr;
        GCancellable *cancellable = nullptr;

        std::vector<Monitor> _monitors;

        static void callback(GObject *, GAsyncResult *, gpointer);

        void take_screenshot(GObject *object, GAsyncResult *result);
        void process_screenshot(GdkPixbuf *);
};
