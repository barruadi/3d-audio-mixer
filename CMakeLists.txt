cmake_minimum_required(VERSION 3.26)
project(3D_Audio_Mixer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_executable(3D_Audio_Mixer
    src/main.cc
)


target_include_directories(3D_Audio_Mixer PRIVATE
    ${CMAKE_SOURCE_DIR}/dependencies
)


set_target_properties(3D_Audio_Mixer PROPERTIES
    INSTALL_RPATH "@executable_path/../lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)


target_link_libraries(3D_Audio_Mixer PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/libglfw.3.4.dylib
)


add_custom_command(TARGET 3D_Audio_Mixer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:3D_Audio_Mixer>/../lib
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/lib/libglfw.3.4.dylib
        $<TARGET_FILE_DIR:3D_Audio_Mixer>/../lib
)
