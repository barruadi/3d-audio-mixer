#pragma once

#include "render/render_base.hh"

namespace nrender
{
    class OpenGL_FrameBuffer : public FrameBuffer
    {
        public:

            void create_buffers(int32_t width, int32_t height) override;

            void delete_buffers() override;

            void bind() override;

            void unbind() override;

            uint32_t get_texture_id()
            {
                return mTextureId;
            }
    };
} // namespace nrender
