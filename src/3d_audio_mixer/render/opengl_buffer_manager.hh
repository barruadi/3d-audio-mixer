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

            uint32_t get_texture_id() override
            {
                return mTextureId;
            }
    };

    class OpenGL_VertexIndexBuffer : public VertexIndexBuffer
    {
        public:
            OpenGL_VertexIndexBuffer(): VertexIndexBuffer()
            {

            }

            void create_buffers(const std::vector<glm::vec3>& nodePosition) override;

            void delete_buffers() override;

            void bind() override;

            void unbind() override;

            void draw() override;

            void draw_line();
    };
} // namespace nrender
