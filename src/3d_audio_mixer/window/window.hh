#pragma once

#include "window/iwindow.hh"
#include "render/opengl_context.hh"

#include <GLFW/glfw3.h>

namespace nrender {
    class OpenGL_Context;
}

namespace nwindow
{
    class Window: public IWindow
    {
        private:
            GLFWwindow* window;

            bool isRunning;

            std::unique_ptr<nrender::OpenGL_Context> mRender;

        public:
            Window(): isRunning(true), window(nullptr)
            {
                mRender = std::make_unique<nrender::OpenGL_Context>();
            }

            bool init(int width, int heigth, const std::string& title);

            void render();

            void handle_input();

            bool is_running() { return isRunning; }

            void* get_native_window() override { return window; };

            void set_native_window(void* window) override
            {
                window = (GLFWwindow*)window;
            }
            
            void on_close() override;

            void on_key(int key, int scancode, int action, int mods) override;
    };
}