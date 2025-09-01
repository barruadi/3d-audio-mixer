#pragma once

#include "render/opengl_buffer_manager.hh"

namespace nelement
{
    class SoundNode
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

            void update(const std::string& file, 
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
                mVertexBuffer->create_buffers(mPosition);
            }
    };
} // namespace nelement
