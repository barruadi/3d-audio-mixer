#define MINIAUDIO_IMPLEMENTATION

#include "audio/audio_system.hh"

namespace naudio
{
    AudioSystem::AudioSystem()
        : mEngine(nullptr)
        , mNextSoundId(0)
    {
    }

    AudioSystem::~AudioSystem()
    {
        shutdown();
    }

    void AudioSystem::attach_engine(ma_engine* engine)
    {
        if (mEngine == engine) return;
        mEngine = engine;
    }

    void AudioSystem::update(const glm::vec3& position)
    {
        for (auto& [id, soundData] : mSounds)
        {
            if (soundData.isLoaded)
            {
                ma_sound_set_position(&soundData.sound, position.x, position.y, position.z);
            }
        }
    }

    void AudioSystem::shutdown()
    {
        unload_all();
        mEngine = nullptr;
    }

    int AudioSystem::load(const std::string& filePath)
    {
        if (!mEngine)
        {
            std::cerr << "[ERROR] AudioSystem: No engine attached" << std::endl;
            return -1;
        }

        // Check if sound is already loaded
        for (const auto& [id, soundData] : mSounds)
        {
            if (soundData.filePath == filePath && soundData.isLoaded)
            {
                std::cout << "[INFO] Sound already loaded: " << filePath << " (ID: " << id << ")" << std::endl;
                return id;
            }
        }

        SoundData soundData;
        soundData.filePath = filePath;
        soundData.isLoaded = false;
        soundData.isPlaying = false;

        ma_result result = ma_sound_init_from_file(
            mEngine,
            filePath.c_str(),
            MA_SOUND_FLAG_DECODE,  // Decode immediately for lower latency
            NULL,
            NULL,
            &soundData.sound
        );

        if (result != MA_SUCCESS)
        {
            std::cerr << "[ERROR] Failed to load sound: " << filePath << std::endl;
            return -1;
        }

        // Set default 3D audio properties
        ma_sound_set_attenuation_model(&soundData.sound, ma_attenuation_model_inverse);
        ma_sound_set_min_distance(&soundData.sound, 1.0f);
        ma_sound_set_max_distance(&soundData.sound, 100.0f);
        ma_sound_set_volume(&soundData.sound, 1.0f);
        ma_sound_set_looping(&soundData.sound, MA_FALSE);

        soundData.isLoaded = true;

        int soundId = mNextSoundId++;
        mSounds[soundId] = std::move(soundData);

        std::cout << "[INFO] Loaded sound: " << filePath << " (ID: " << soundId << ")" << std::endl;
        return soundId;
    }

    void AudioSystem::unload(int soundId)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData) return;

        if (soundData->isLoaded)
        {
            ma_sound_uninit(&soundData->sound);
            std::cout << "[INFO] Unloaded sound ID: " << soundId << std::endl;
        }

        mSounds.erase(soundId);
    }

    void AudioSystem::unload_all()
    {
        for (auto& [id, soundData] : mSounds)
        {
            if (soundData.isLoaded)
            {
                ma_sound_uninit(&soundData.sound);
            }
        }
        mSounds.clear();
        std::cout << "[INFO] Unloaded all sounds" << std::endl;
    }

    void AudioSystem::play(int soundId)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        // Rewind to start if already played
        ma_sound_seek_to_pcm_frame(&soundData->sound, 0);
        ma_sound_start(&soundData->sound);
        soundData->isPlaying = true;

        std::cout << "[INFO] Playing sound ID: " << soundId << std::endl;
    }

    void AudioSystem::stop(int soundId)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_stop(&soundData->sound);
        ma_sound_seek_to_pcm_frame(&soundData->sound, 0);
        soundData->isPlaying = false;

        std::cout << "[INFO] Stopped sound ID: " << soundId << std::endl;
    }

    void AudioSystem::pause(int soundId)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_stop(&soundData->sound);
        soundData->isPlaying = false;
    }

    void AudioSystem::resume(int soundId)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_start(&soundData->sound);
        soundData->isPlaying = true;
    }

    void AudioSystem::set_position(int soundId, const glm::vec3& position)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_set_position(&soundData->sound, position.x, position.y, position.z);
    }

    void AudioSystem::set_volume(int soundId, float volume)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_set_volume(&soundData->sound, volume);
    }

    void AudioSystem::set_looping(int soundId, bool looping)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_set_looping(&soundData->sound, looping ? MA_TRUE : MA_FALSE);
    }

    void AudioSystem::set_pan(int soundId, float pan)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_set_pan(&soundData->sound, pan);
    }

    void AudioSystem::set_pitch(int soundId, float pitch)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_set_pitch(&soundData->sound, pitch);
    }

    void AudioSystem::set_min_distance(int soundId, float distance)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_set_min_distance(&soundData->sound, distance);
    }

    void AudioSystem::set_max_distance(int soundId, float distance)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_set_max_distance(&soundData->sound, distance);
    }

    void AudioSystem::set_attenuation(int soundId, ma_attenuation_model model)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_set_attenuation_model(&soundData->sound, model);
    }

    float AudioSystem::get_cursor_seconds(int soundId)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return 0.0f;

        float cursor = 0.0f;
        ma_sound_get_cursor_in_seconds(&soundData->sound, &cursor);
        return cursor;
    }

    float AudioSystem::get_length_seconds(int soundId)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return 0.0f;

        float length = 0.0f;
        ma_sound_get_length_in_seconds(&soundData->sound, &length);
        return length;
    }

    void AudioSystem::seek_to_second(int soundId, float seconds)
    {
        SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return;

        ma_sound_seek_to_second(&soundData->sound, seconds);
    }

    bool AudioSystem::is_playing(int soundId) const
    {
        const SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return false;

        return ma_sound_is_playing(&soundData->sound);
    }

    bool AudioSystem::is_loaded(int soundId) const
    {
        const SoundData* soundData = get_sound(soundId);
        return soundData && soundData->isLoaded;
    }

    float AudioSystem::get_volume(int soundId) const
    {
        const SoundData* soundData = get_sound(soundId);
        if (!soundData || !soundData->isLoaded) return 0.0f;

        return ma_sound_get_volume(&soundData->sound);
    }

    SoundData* AudioSystem::get_sound(int soundId)
    {
        auto it = mSounds.find(soundId);
        if (it == mSounds.end())
        {
            return nullptr;
        }
        return &it->second;
    }

    const SoundData* AudioSystem::get_sound(int soundId) const
    {
        auto it = mSounds.find(soundId);
        if (it == mSounds.end())
        {
            return nullptr;
        }
        return &it->second;
    }
} // namespace naudio
