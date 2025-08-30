#pragma once

#include "window/iwindow.hh"

namespace nrender
{
    class RenderContext
    {
        protected:
            nwindow::IWindow* mWindow;
        
        public:
            RenderContext() : mWindow(nullptr) {}

            virtual bool init(nwindow::IWindow* window)
            {
                mWindow = window;
                return true;
            }

            virtual void pre_render() = 0;

            virtual void post_render() = 0;

            virtual void end() = 0;
    };
} // namespace nrender
