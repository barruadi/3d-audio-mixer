#pragma once

#include "render/opengl_buffer_manager.hh"
#include "elements/element.hh"

namespace nelement
{
    class Grid: public Element
    {
        private:
            std::vector<glm::vec3> mPosition;            

            std::shared_ptr<nrender::OpenGL_VertexIndexBuffer> mVertexBuffer;

        public:
            // vertices per axis: shaft (2) + arrowhead lines at the positive tip (4)
            static constexpr int cVerticesPerAxis = 6;

            Grid()
            {
                mPosition = {
                    // X axis
                    { -2.0f,  0.0f,  0.0f }, {  2.0f,  0.0f,  0.0f },
                    {  2.0f,  0.0f,  0.0f }, {  1.8f,  0.12f, 0.0f },
                    {  2.0f,  0.0f,  0.0f }, {  1.8f, -0.12f, 0.0f },
                    // Y axis
                    {  0.0f, -2.0f,  0.0f }, {  0.0f,  2.0f,  0.0f },
                    {  0.0f,  2.0f,  0.0f }, {  0.12f, 1.8f,  0.0f },
                    {  0.0f,  2.0f,  0.0f }, { -0.12f, 1.8f,  0.0f },
                    // Z axis
                    {  0.0f,  0.0f, -2.0f }, {  0.0f,  0.0f,  2.0f },
                    {  0.0f,  0.0f,  2.0f }, {  0.12f, 0.0f,  1.8f },
                    {  0.0f,  0.0f,  2.0f }, { -0.12f, 0.0f,  1.8f }
                };
                init();
            }

            void init()
            {
                mVertexBuffer = std::make_shared<nrender::OpenGL_VertexIndexBuffer>();
                create_buffer();
            }

            virtual void render()
            {
                mVertexBuffer->draw_lines(0, static_cast<int>(mPosition.size()));
            }

            void render_axis(int axis)
            {
                mVertexBuffer->draw_lines(axis * cVerticesPerAxis, cVerticesPerAxis);
            }

            void update(nshader::Shader* shader)
            {
                // shader->use();
            }

            void create_buffer()
            {
                mVertexBuffer->create_buffers(mPosition);
            }
    };
} // namespace nelement