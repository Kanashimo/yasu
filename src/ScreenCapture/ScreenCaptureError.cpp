#include "ScreenCaptureError.h"
#include "glib.h"

GQuark yasu_screencapture_error_quark()
{
    return g_quark_from_static_string("yasu-screencapture-error-quark");
}
