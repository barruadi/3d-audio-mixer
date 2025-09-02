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
            Grid()
            {
                mPosition = {
                    { -2.0f,  0.0f,  0.0f },
                    {  2.0f,  0.0f,  0.0f },
                    {  0.0f, -2.0f,  0.0f },
                    {  0.0f,  2.0f,  0.0f },
                    {  0.0f,  0.0f, -2.0f },
                    {  0.0f,  0.0f,  2.0f }
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
                mVertexBuffer->draw_line();
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