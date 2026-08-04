cmake_minimum_required(VERSION 3.26)
project(3D_Audio_Mixer VERSION 1.0.0)

# ── Google Test ──────────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_SUBDIR googletest-1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenGL REQUIRED)

add_executable(3D_Audio_Mixer
    src/main.cc
    src/glad.c
    
    # main
    src/3d_audio_mixer/render/opengl_buffer_manager.cc
    src/3d_audio_mixer/render/opengl_context.cc
    src/3d_audio_mixer/render/ui_context.cc
    src/3d_audio_mixer/audio/audio_context.cc
    src/3d_audio_mixer/audio/audio_system.cc
    src/3d_audio_mixer/shader/shader.cc
    src/3d_audio_mixer/app/application.cc
    src/3d_audio_mixer/window/window.cc

    # panels
    src/3d_audio_mixer/ui/menu_panel.cc
    src/3d_audio_mixer/ui/scene_view.cc
    src/3d_audio_mixer/ui/node_info.cc
    src/3d_audio_mixer/ui/timeline_panel.cc

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
    ${CMAKE_SOURCE_DIR}/dependencies/MiniAudio
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


# ── Unit Tests ───────────────────────────────────────────────────────────────
add_executable(3D_Audio_Mixer_Tests
    src/glad.c
    src/3d_audio_mixer/audio/audio_system.cc
    src/3d_audio_mixer/audio/audio_context.cc
    src/3d_audio_mixer/render/opengl_buffer_manager.cc
    src/3d_audio_mixer/shader/shader.cc

    src/test/elements/camera.test.cc
    src/test/elements/sound_node.test.cc
    src/test/elements/listener.test.cc
    src/test/utils/file_io.test.cc
    src/test/utils/scene_loader.test.cc
)

target_include_directories(3D_Audio_Mixer_Tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/3d_audio_mixer
    ${CMAKE_SOURCE_DIR}/dependencies
    ${CMAKE_SOURCE_DIR}/dependencies/imgui
    ${CMAKE_SOURCE_DIR}/dependencies/imgui/backends
    ${CMAKE_SOURCE_DIR}/dependencies/MiniAudio
)

target_link_libraries(3D_Audio_Mixer_Tests PRIVATE
    GTest::gtest_main
    ${CMAKE_SOURCE_DIR}/lib/libglfw.3.4.dylib
    OpenGL::GL
)

if(APPLE)
    target_compile_definitions(3D_Audio_Mixer_Tests PRIVATE GL_SILENCE_DEPRECATION)
endif()

include(GoogleTest)
gtest_discover_tests(3D_Audio_Mixer_Tests)

# ─────────────────────────────────────────────────────────────────────────────
add_custom_command(TARGET 3D_Audio_Mixer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:3D_Audio_Mixer>/../lib
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/lib/libglfw.3.4.dylib
        $<TARGET_FILE_DIR:3D_Audio_Mixer>/../lib
)
