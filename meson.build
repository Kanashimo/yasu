project('yasu', 'cpp', default_options: ['cpp_std=c++20'])

add_global_arguments(
  '-DGLEW_NO_GLU',
  '-DGLEW_EGL',
  language: 'c'
)

add_project_arguments(
  '-DGLEW_NO_GLU',
   '-DGLEW_EGL',
  language: 'cpp'
)

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
    'src/Application.cpp',
    'src/ScreenCapture/ScreenCapture.cpp',
    'src/ScreenCapture/ScreenCaptureError.cpp',
    'src/ShaderLoader/ShaderLoader.cpp',
    'src/UI/BackgroundTexture.cpp',
    'src/UI/Crop.cpp'
)

shaders = files(
  'src/Shaders/vert.glsl',
  'src/Shaders/frag.glsl'
)

fs = import('fs')

foreach f : shaders
  fs.copyfile(f)
endforeach

executable(
    'yasu',
    sources,
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
