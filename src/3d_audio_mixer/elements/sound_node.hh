#pragma once

#include "render/opengl_buffer_manager.hh"
#include "elements/element.hh"

namespace nelement
{
    class SoundNode: public Element
    {
        private:
            std::string mFile;

            // properties
            glm::vec3 mPosition;
            float mVolume;
            float mPan;

            std::shared_ptr<nrender::OpenGL_VertexIndexBuffer> mVertexBuffer;

        public:
            SoundNode():
                mFile(""),
                mPosition(0.0f, 0.0f, 0.0f),
                mVolume(1.0f),
                mPan(0.0f)
            {

            }

            void init()
            {
                mVertexBuffer = std::make_shared<nrender::OpenGL_VertexIndexBuffer>();
                create_buffer();
            }

            virtual void render()
            {
                mVertexBuffer->draw();
            }

            void update(nshader::Shader* shader)
            {
                // shader->use();
            }

            void self_update(const std::string& file, 
                        const glm::vec3& position, 
                        float volume, 
                        float pan)
            {
                mFile = file;
                mPosition = position;
                mVolume = volume;
                mPan = pan;
            }

            void create_buffer()
            {
                std::vector<glm::vec3> positions = {mPosition};
                mVertexBuffer->create_buffers(positions);
            }
    };
} // namespace nelement
