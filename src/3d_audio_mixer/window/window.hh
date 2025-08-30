#pragma once

#include "window/iwindow.hh"
#include "render/opengl_context.hh"
#include "render/ui_context.hh"

#include <GLFW/glfw3.h>

namespace nrender {
    class OpenGL_Context;
    class UI_Context;
}

namespace nwindow
{
    class Window: public IWindow
    {
        private:
            GLFWwindow* mWindow;

            bool isRunning;

            std::unique_ptr<nrender::OpenGL_Context> mRender;
            std::unique_ptr<nrender::UI_Context> mUI;

        public:
            Window(): isRunning(true), mWindow(nullptr)
            {
                mRender = std::make_unique<nrender::OpenGL_Context>();
                mUI = std::make_unique<nrender::UI_Context>();
            }

            bool init(int width, int height, const std::string& title);

            void render();

            void handle_input();

            bool is_running() { return isRunning; }

            void* get_native_window() override { return mWindow; };

            void set_native_window(void* window) override
            {
                mWindow = (GLFWwindow*)window;
            }
            
            void on_close() override;

            void on_key(int key, int scancode, int action, int mods) override;

            void on_resize(int width, int height) override;

            void on_scroll(double delta) override;
    };
}