#pragma once

#include "audio/spatial_source.hh"
#include <string>
#include <vector>
#include <unordered_map>

// OpenAL opaque handle types — stored as void* so this header stays AL-free.
// spatial_engine.cc includes the actual AL headers and casts appropriately.

namespace naudio
{
    // SpatialEngine: offline spatial audio renderer using OpenAL Soft.
    //
    // Uses the ALC_SOFT_loopback device — no real audio hardware required.
    // Supports HRTF (ALC_SOFT_HRTF) and environmental reverb (ALC_EXT_EFX).
    // Audio files are decoded via MiniAudio; output is written as a stereo WAV.
    //
    // Typical usage:
    //   SpatialEngine engine;
    //   engine.init();
    //   engine.add_source({ "kick.wav", {1,0,0}, 0.8f });
    //   engine.set_listener_position({0,0,0});
    //   engine.render_to_wav("out.wav", 5.0f);
    //   engine.shutdown();
    class SpatialEngine
    {
    public:
        SpatialEngine();
        ~SpatialEngine();

        // Not copyable — owns AL device/context handles.
        SpatialEngine(const SpatialEngine&) = delete;
        SpatialEngine& operator=(const SpatialEngine&) = delete;

        // Open the loopback device and create an AL context.
        // enableHRTF: request ALC_SOFT_HRTF — falls back gracefully if unavailable.
        // Returns false if the loopback device or context cannot be created.
        bool init(bool enableHRTF = true);

        void shutdown();

        // Add a source to the pending render list.
        // Must be called before render_to_wav; ignored after it.
        void add_source(const SpatialSource& source);

        // Remove all pending sources.
        void clear_sources();

        // Listener position (default: origin).
        void set_listener_position(const glm::vec3& position);

        // Listener facing direction (at) and world-up vector.
        // Defaults: at=(0,0,-1), up=(0,1,0).
        void set_listener_orientation(const glm::vec3& at, const glm::vec3& up);

        // Apply an EFX reverb preset to all sources.
        // Requires ALC_EXT_EFX — silently does nothing if unavailable.
        void set_reverb_preset(ReverbPreset preset);

        // Render all sources to a stereo 16-bit WAV file.
        // durationSeconds: length of output audio (must be > 0).
        // sampleRate: output sample rate (default 48 000 Hz).
        // Returns false if init() was not called or writing fails.
        bool render_to_wav(const std::string& outputPath,
                           float durationSeconds,
                           int sampleRate = 48000);

        bool is_initialized()    const { return mInitialized; }
        bool is_hrtf_enabled()   const { return mHRTFEnabled; }
        bool is_reverb_enabled() const { return mReverbEnabled; }

    private:
        void* mDevice  = nullptr;   // ALCdevice*
        void* mContext = nullptr;   // ALCcontext*

        bool mInitialized  = false;
        bool mHRTFEnabled  = false;
        bool mReverbEnabled = false;

        unsigned int mEffect     = 0;   // ALuint
        unsigned int mEffectSlot = 0;   // ALuint

        glm::vec3 mListenerPos = { 0.0f, 0.0f, 0.0f };
        glm::vec3 mListenerAt  = { 0.0f, 0.0f, -1.0f };
        glm::vec3 mListenerUp  = { 0.0f, 1.0f, 0.0f };

        std::vector<SpatialSource> mPendingSources;

        void apply_listener();
        void setup_reverb(ReverbPreset preset);
        void teardown_reverb();
    };
} // namespace naudio
