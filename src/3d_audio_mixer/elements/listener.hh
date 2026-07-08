#pragma once

#include "app/pch.h"
#include "elements/element.hh"
#include "render/opengl_buffer_manager.hh"
#include "audio/audio_context.hh"

#include <string>

namespace nelement
{
    class Listener: public Element
    {
        private:
            std::string mName;

            glm::vec3 mPosition;

            std::shared_ptr<nrender::OpenGL_VertexIndexBuffer> mVertexBuffer;

            // shared engine context; drives ma_engine listener 0
            std::shared_ptr<naudio::AudioContext> mAudioContext;

        public:
            Listener()
                : mName("listener")
                , mPosition(0.0f, 0.0f, 0.0f)
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

            virtual void update(nshader::Shader* shader) override
            {
            }

            void create_buffer()
            {
                std::vector<glm::vec3> positions = {mPosition};
                mVertexBuffer->create_buffers(positions);
            }

            void set_engine(const std::shared_ptr<naudio::AudioContext>& audioContext)
            {
                mAudioContext = audioContext;
                apply_to_engine();
            }

            void set_position(const glm::vec3& position)
            {
                mPosition = position;
                apply_to_engine();
                // Update visual buffer
                create_buffer();
            }

            // [TODO]: placeholder orientation until the audio engine calculates
            // spatial sound from a configurable listener direction
            void apply_to_engine()
            {
                if (mAudioContext)
                {
                    mAudioContext->set_listener_position(mPosition,
                        glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }

            // Getters
            const std::string& get_name() const
            {
                return mName;
            }

            glm::vec3 get_position() const
            {
                return mPosition;
            }
    };
} // namespace nelement
