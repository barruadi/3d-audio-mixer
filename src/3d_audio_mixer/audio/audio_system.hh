#pragma once

#include <miniaudio.h>
#include <unordered_map>

#include "app/pch.h"

namespace naudio
{
    struct SoundData
    {
        ma_sound sound;
        std::string filePath;
        bool isLoaded;
        bool isPlaying;
    };

    class AudioSystem
    {
        private:
            ma_engine* mEngine;
            std::unordered_map<int, SoundData> mSounds;
            int mNextSoundId;

        public:
            AudioSystem();
            ~AudioSystem();

            // Prevent copying - ma_sound resources cannot be shallow-copied
            AudioSystem(const AudioSystem&) = delete;
            AudioSystem& operator=(const AudioSystem&) = delete;

            // Main Functions
            void attach_engine(ma_engine* engine);
            void update(const glm::vec3& position);
            void shutdown();

            // Sound Management
            int load(const std::string& filePath);
            void unload(int soundId);
            void unload_all();

            // Playback Controls
            void play(int soundId);
            void stop(int soundId);
            void pause(int soundId);
            void resume(int soundId);

            // Properties
            void set_position(int soundId, const glm::vec3& position);
            void set_volume(int soundId, float volume);
            void set_looping(int soundId, bool looping);
            void set_pitch(int soundId, float pitch);
            void set_min_distance(int soundId, float distance);
            void set_max_distance(int soundId, float distance);
            void set_attenuation(int soundId, ma_attenuation_model model);

            // Getters
            bool is_playing(int soundId) const;
            bool is_loaded(int soundId) const;
            float get_volume(int soundId) const;

        private:
            SoundData* get_sound(int soundId);
            const SoundData* get_sound(int soundId) const;
    };
} // namespace naudio
