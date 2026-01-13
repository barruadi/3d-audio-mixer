#pragma once

#include "app/pch.h"
#include "elements/element.hh"
#include "audio/audio_context.hh"

#include <string>

namespace nelement
{
    class Listener: public Element
    {
        private:
            std::string id;

            glm::vec3 mPosition;
            std::shared_ptr<naudio::AudioContext> mAudioContext;

        public:
            Listener()
        {
            id = "listener";
            mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        void init()
        {
            mAudioContext = std::make_shared<naudio::AudioContext>();
        }

        virtual void render()
        {
            
        }

        void update(nshader::Shader* shader)
        {
            // shader->use();
        }

        void self_update(const glm::vec3& position)
        {
            mPosition = position;
            if (mAudioContext)
            {
                mAudioContext->set_listener_position(mPosition, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
        }
        
        const glm::vec3& get_position() const
        {
            return mPosition;
        }
    };
} // namespace nelement
