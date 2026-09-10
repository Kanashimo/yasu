project('yasu', 'cpp')

glib = dependency('glib-2.0')
gio = dependency('gio-2.0')
libportal = dependency('libportal')
gdk_pixbuf = dependency('gdk-pixbuf-2.0')
gtk = dependency('gtk4')
glfw = dependency('glfw3')
imgui = dependency('imgui')
gl = dependency('gl')
glew = dependency('glew')

sources = files(
    'src/main.cpp',
    'src/ScreenCapture/ScreenCapture.cpp',
    'src/ScreenCapture/ScreenCaptureError.cpp'
)

headers = files(
    'src/ScreenCapture/ScreenCapture.h',
    'src/ScreenCapture/ScreenCaptureError.h'
)

executable(
    'yasu',
    sources,
    headers,
    dependencies: [
        glib,
        gio,
        libportal,
        gdk_pixbuf,
        gtk,
        glfw,
        imgui,
        gl,
        glew
    ]
)
