cmake_minimum_required(VERSION 3.26)
project(3D_Audio_Mixer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenGL REQUIRED)

add_executable(3D_Audio_Mixer
    src/main.cc
    src/3d_audio_mixer/render/opengl_context.cc
    src/3d_audio_mixer/render/ui_context.cc
    src/3d_audio_mixer/app/application.cc
    src/3d_audio_mixer/window/window.cc

    ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/backends/imgui_impl_opengl3.cpp
)


if(APPLE)
    target_compile_definitions(3D_Audio_Mixer PRIVATE GL_SILENCE_DEPRECATION)
endif()


target_include_directories(3D_Audio_Mixer PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies
    ${CMAKE_SOURCE_DIR}/dependencies/imgui
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/backends
    ${CMAKE_SOURCE_DIR}/src/3d_audio_mixer
)


set_target_properties(3D_Audio_Mixer PROPERTIES
    INSTALL_RPATH "@executable_path/../lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)


target_link_libraries(3D_Audio_Mixer PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/libglfw.3.4.dylib
    OpenGL::GL
)


add_custom_command(TARGET 3D_Audio_Mixer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:3D_Audio_Mixer>/../lib
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/lib/libglfw.3.4.dylib
        $<TARGET_FILE_DIR:3D_Audio_Mixer>/../lib
)
