project('yasu', 'cpp', default_options: ['cpp_std=c++20'])

add_project_arguments(
  '-DGLFW_INCLUDE_NONE',
  language: 'cpp'
)

glib = dependency('glib-2.0')
gio = dependency('gio-2.0')
libportal = dependency('libportal')
gdk_pixbuf = dependency('gdk-pixbuf-2.0')
gtk = dependency('gtk4')
glfw = dependency('glfw3')
imgui = dependency('imgui', version: '>=1.92.6')
glbinding = dependency('glbinding')

sources = files(
    'src/main.cpp',
    'src/Application.cpp',
    'src/ScreenCapture/ScreenCapture.cpp',
    'src/ScreenCapture/ScreenCaptureError.cpp',
    'src/ShaderLoader/ShaderLoader.cpp',
    'src/UI/BackgroundTexture.cpp',
    'src/UI/Crop.cpp',
    'src/UI/Toolbar.cpp',
    'src/UI/Pipette.cpp'
)

shaders = files(
  'src/Shaders/vert.glsl',
  'src/Shaders/frag.glsl'
)

fs = import('fs')

foreach f : shaders
  fs.copyfile(f)
endforeach

bash = find_program('bash', required: true)

find_program('xxd', required: true)
find_program('sed', required: true)

## VERY VERY VERY VERY TEMPORARY SOLUTION, I DON'T HAVE TIME FOR THIS
roboto_regular = custom_target(
  'build_fonts1',
  input: 'src/Fonts/Roboto-Regular.ttf',
  output: 'Roboto_Regular.h',
  command: [bash, files('tools/build_font.sh'), '@INPUT@', '@OUTPUT@'],
)

roboto_bold = custom_target(
  'build_fonts2',
  input: 'src/Fonts/Roboto-Bold.ttf',
  output: 'Roboto_Bold.h',
  command: [bash, files('tools/build_font.sh'), '@INPUT@', '@OUTPUT@'],
)

roboto_light = custom_target(
  'build_fonts3',
  input: 'src/Fonts/Roboto-Light.ttf',
  output: 'Roboto_Light.h',
  command: [bash, files('tools/build_font.sh'), '@INPUT@', '@OUTPUT@'],
)

roboto_light = custom_target(
  'build_fonts4',
  input: 'src/Fonts/Icons.ttf',
  output: 'Icons.h',
  command: [bash, files('tools/build_font.sh'), '@INPUT@', '@OUTPUT@'],
)

executable(
    'yasu',
    sources,
    roboto_regular,
    roboto_bold,
    roboto_light,
    dependencies: [
        glib,
        gio,
        libportal,
        gdk_pixbuf,
        gtk,
        glfw,
        imgui,
        glbinding
    ]
)
