#include "audio/audio_context.hh"

namespace naudio
{
    bool AudioContext::init()
    {
        ma_result result = ma_engine_init(NULL, &mEngine);
        if (result != MA_SUCCESS) 
        {
            return false;
        }
        return true;
    }

    void AudioContext::shutdown()
    {
        ma_engine_uninit(&mEngine);
    }

    void AudioContext::set_listener_position(const glm::vec3& position,
                                            const glm::vec3& direction,
                                            const glm::vec3& up)
    {
        ma_engine_listener_set_position(&mEngine, 0, position.x, position.y, position.z);
        ma_engine_listener_set_direction(&mEngine, 0, direction.x, direction.y, direction.z);
        ma_engine_listener_set_world_up(&mEngine, 0, up.x, up.y, up.z);
    }
} // namespace naudio
