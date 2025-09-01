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

    class FrameBuffer
    {
        protected:
            uint32_t mFBO       = 0;
            uint32_t mTextureId = 0;
            uint32_t mDepthId   = 0;
            int32_t mWidth      = 0;
            int32_t mHeight     = 0;

        public:
            FrameBuffer():
                mFBO{0}, mDepthId{0}
            {

            }

            virtual void create_buffers(int32_t width, int32_t height) = 0;

            virtual void delete_buffers() = 0;

            virtual void bind() = 0;

            virtual void unbind() = 0;

            virtual uint32_t get_texture_id() = 0;
    };
} // namespace nrender
