#pragma once
#include <memory>
#include "audio/audio_context.hh"

namespace naudio
{
    struct Services
    {
        std::shared_ptr<naudio::AudioContext> sAudioContext;
    
        static Services& instance()
        {
            static Services instance;
            return instance;
        }
    };

    inline std::shared_ptr<naudio::AudioContext> get_audio_context()
    {
        std::cout << "[INFO] Getting audio context: " << Services::instance().sAudioContext << std::endl;
        return Services::instance().sAudioContext;
    }
}