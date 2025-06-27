#pragma once

#include <string>

namespace nwindow
{
    class IWindow
    {
        public:
            std::string Title;
            int Width;
            int Height;

            virtual void* get_native_window() = 0;

            virtual void set_native_window(void* window) = 0;

            virtual void on_close() = 0;

            virtual void on_key(int key, int scancode, int action, int mods) = 0;

    };
} // namespace window
