project('yasu', 'cpp')

glib = dependency('glib-2.0')
gio = dependency('gio-2.0')
libportal = dependency('libportal')
gdk_pixbuf = dependency('gdk-pixbuf-2.0')
gtk = dependency('gtk4')
glfw = dependency('glfw3')
imgui = dependency('imgui', version: '>=1.92.6')
gl = dependency('gl')
egl = dependency('egl')

# force using wrap for glew dep instead of a system one
glew = subproject('glew').get_variable('glew_dep')

sources = files(
    'src/main.cpp',
    'src/ScreenCapture/ScreenCapture.cpp',
    'src/ScreenCapture/ScreenCaptureError.cpp',
    'src/ShaderLoader/ShaderLoader.cpp'
)

headers = files(
    'src/ScreenCapture/ScreenCapture.h',
    'src/ScreenCapture/ScreenCaptureError.h',
    'src/ShaderLoader/ShaderLoader.h'
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
        glew,
        egl
    ]
)
