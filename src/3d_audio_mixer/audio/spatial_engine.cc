#include "audio/spatial_engine.hh"

// OpenAL Soft headers — must come before miniaudio to avoid macro conflicts.
// AL_ALEXT_PROTOTYPES exposes alcLoopbackOpenDeviceSOFT / alcRenderSamplesSOFT.
#define AL_ALEXT_PROTOTYPES 1
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-presets.h>

// MiniAudio decoder and encoder (implementation lives in audio_system.cc).
#include <miniaudio.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

namespace naudio
{
    // ── EFX function pointers (loaded on demand) ──────────────────────────────
    static LPALGENEFFECTS        alGenEffects_       = nullptr;
    static LPALDELETEEFFECTS     alDeleteEffects_    = nullptr;
    static LPALISEFFECT          alIsEffect_         = nullptr;
    static LPALEFFECTI           alEffecti_          = nullptr;
    static LPALEFFECTF           alEffectf_          = nullptr;
    static LPALGENFILTERS        alGenFilters_       = nullptr;
    static LPALDELETEFILTERS     alDeleteFilters_    = nullptr;
    static LPALFILTERI           alFilteri_          = nullptr;
    static LPALGENAUXILIARYEFFECTSLOTS    alGenAuxiliaryEffectSlots_    = nullptr;
    static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots_ = nullptr;
    static LPALAUXILIARYEFFECTSLOTI       alAuxiliaryEffectSloti_       = nullptr;

    static bool sEFXLoaded = false;

    static bool load_efx_functions()
    {
        if (sEFXLoaded) return true;

        #define LOAD(T, N) N##_ = (T)alGetProcAddress(#N); if (!N##_) return false;
        LOAD(LPALGENEFFECTS,                 alGenEffects)
        LOAD(LPALDELETEEFFECTS,              alDeleteEffects)
        LOAD(LPALISEFFECT,                   alIsEffect)
        LOAD(LPALEFFECTI,                    alEffecti)
        LOAD(LPALEFFECTF,                    alEffectf)
        LOAD(LPALGENFILTERS,                 alGenFilters)
        LOAD(LPALDELETEFILTERS,              alDeleteFilters)
        LOAD(LPALFILTERI,                    alFilteri)
        LOAD(LPALGENAUXILIARYEFFECTSLOTS,    alGenAuxiliaryEffectSlots)
        LOAD(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots)
        LOAD(LPALAUXILIARYEFFECTSLOTI,       alAuxiliaryEffectSloti)
        #undef LOAD

        sEFXLoaded = true;
        return true;
    }

    // ── Constructor / Destructor ──────────────────────────────────────────────
    // Convenience casts so the body reads cleanly.
    #define ALDEV  reinterpret_cast<ALCdevice*>(mDevice)
    #define ALCTX  reinterpret_cast<ALCcontext*>(mContext)

    SpatialEngine::SpatialEngine() = default;

    SpatialEngine::~SpatialEngine()
    {
        shutdown();
    }

    // ── init ─────────────────────────────────────────────────────────────────
    bool SpatialEngine::init(bool enableHRTF)
    {
        if (mInitialized)
        {
            std::cerr << "[ERROR] SpatialEngine already initialized" << std::endl;
            return false;
        }

        // Open loopback device — no real audio hardware needed.
        mDevice = static_cast<void*>(alcLoopbackOpenDeviceSOFT(nullptr));
        if (!mDevice)
        {
            std::cerr << "[ERROR] SpatialEngine: failed to open loopback device" << std::endl;
            return false;
        }

        // Create context at 48 kHz stereo 16-bit with optional HRTF.
        // The actual sample-rate is overridden per-render but we need a valid context.
        ALCint attrs[] = {
            ALC_FORMAT_CHANNELS_SOFT, ALC_STEREO_SOFT,
            ALC_FORMAT_TYPE_SOFT,     ALC_SHORT_SOFT,
            ALC_FREQUENCY,            48000,
            ALC_HRTF_SOFT,            enableHRTF ? ALC_TRUE : ALC_FALSE,
            0
        };

        mContext = static_cast<void*>(alcCreateContext(ALDEV, attrs));
        if (!mContext)
        {
            std::cerr << "[ERROR] SpatialEngine: failed to create AL context" << std::endl;
            alcCloseDevice(ALDEV);
            mDevice = nullptr;
            return false;
        }

        alcMakeContextCurrent(ALCTX);

        // Query HRTF status.
        ALCint hrtfStatus = 0;
        alcGetIntegerv(ALDEV, ALC_HRTF_STATUS_SOFT, 1, &hrtfStatus);
        mHRTFEnabled = (hrtfStatus == ALC_HRTF_ENABLED_SOFT);

        mInitialized = true;

        std::cout << "[INFO] SpatialEngine initialized. HRTF: "
                  << (mHRTFEnabled ? "enabled" : "not available") << std::endl;

        apply_listener();
        return true;
    }

    // ── shutdown ──────────────────────────────────────────────────────────────
    void SpatialEngine::shutdown()
    {
        if (!mInitialized) return;

        teardown_reverb();

        alcMakeContextCurrent(nullptr);
        if (mContext) { alcDestroyContext(ALCTX); mContext = nullptr; }
        if (mDevice)  { alcCloseDevice(ALDEV);    mDevice  = nullptr; }

        mInitialized  = false;
        mHRTFEnabled  = false;
        mReverbEnabled = false;
        sEFXLoaded    = false;

        std::cout << "[INFO] SpatialEngine shutdown" << std::endl;
    }

    // ── source management ─────────────────────────────────────────────────────
    void SpatialEngine::add_source(const SpatialSource& source)
    {
        mPendingSources.push_back(source);
    }

    void SpatialEngine::clear_sources()
    {
        mPendingSources.clear();
    }

    // ── listener ──────────────────────────────────────────────────────────────
    void SpatialEngine::set_listener_position(const glm::vec3& position)
    {
        mListenerPos = position;
        if (mInitialized) apply_listener();
    }

    void SpatialEngine::set_listener_orientation(const glm::vec3& at, const glm::vec3& up)
    {
        mListenerAt = at;
        mListenerUp = up;
        if (mInitialized) apply_listener();
    }

    void SpatialEngine::apply_listener()
    {
        alListener3f(AL_POSITION, mListenerPos.x, mListenerPos.y, mListenerPos.z);

        ALfloat orient[6] = {
            mListenerAt.x, mListenerAt.y, mListenerAt.z,
            mListenerUp.x, mListenerUp.y, mListenerUp.z
        };
        alListenerfv(AL_ORIENTATION, orient);
        alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    }

    // ── reverb ────────────────────────────────────────────────────────────────
    void SpatialEngine::set_reverb_preset(ReverbPreset preset)
    {
        if (!mInitialized) return;

        if (preset == ReverbPreset::None)
        {
            teardown_reverb();
            return;
        }

        if (!alcIsExtensionPresent(ALDEV, "ALC_EXT_EFX"))
        {
            std::cerr << "[WARN] SpatialEngine: EFX extension not available" << std::endl;
            return;
        }

        if (!load_efx_functions()) return;

        teardown_reverb();
        setup_reverb(preset);
    }

    void SpatialEngine::setup_reverb(ReverbPreset preset)
    {
        alGenAuxiliaryEffectSlots_(1, &mEffectSlot);
        alGenEffects_(1, &mEffect);

        alEffecti_(mEffect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);

        // Pick preset parameters.
        EFXEAXREVERBPROPERTIES props = EFX_REVERB_PRESET_GENERIC;
        switch (preset)
        {
            case ReverbPreset::SmallRoom:
                props = EFX_REVERB_PRESET_ROOM;        break;
            case ReverbPreset::LargeHall:
                props = EFX_REVERB_PRESET_CONCERTHALL; break;
            case ReverbPreset::Cave:
                props = EFX_REVERB_PRESET_CAVE;        break;
            default: break;
        }

        alEffectf_(mEffect, AL_EAXREVERB_DENSITY,           props.flDensity);
        alEffectf_(mEffect, AL_EAXREVERB_DIFFUSION,         props.flDiffusion);
        alEffectf_(mEffect, AL_EAXREVERB_GAIN,              props.flGain);
        alEffectf_(mEffect, AL_EAXREVERB_GAINHF,            props.flGainHF);
        alEffectf_(mEffect, AL_EAXREVERB_DECAY_TIME,        props.flDecayTime);
        alEffectf_(mEffect, AL_EAXREVERB_REFLECTIONS_GAIN,  props.flReflectionsGain);
        alEffectf_(mEffect, AL_EAXREVERB_LATE_REVERB_GAIN,  props.flLateReverbGain);

        alAuxiliaryEffectSloti_(mEffectSlot, AL_EFFECTSLOT_EFFECT, (ALint)mEffect);
        mReverbEnabled = true;
    }

    void SpatialEngine::teardown_reverb()
    {
        if (!sEFXLoaded) return;

        if (mEffectSlot) { alDeleteAuxiliaryEffectSlots_(1, &mEffectSlot); mEffectSlot = 0; }
        if (mEffect)     { alDeleteEffects_(1, &mEffect);                  mEffect = 0; }
        mReverbEnabled = false;
    }

    // ── render_to_wav ─────────────────────────────────────────────────────────
    bool SpatialEngine::render_to_wav(const std::string& outputPath,
                                      float durationSeconds,
                                      int sampleRate)
    {
        if (!mInitialized)
        {
            std::cerr << "[ERROR] SpatialEngine::render_to_wav: not initialized" << std::endl;
            return false;
        }
        if (durationSeconds <= 0.0f)
        {
            std::cerr << "[ERROR] SpatialEngine::render_to_wav: durationSeconds must be > 0" << std::endl;
            return false;
        }

        // ── Step 1: decode each source file into PCM and create AL buffers/sources ──
        struct LoadedSource
        {
            ALuint alBuffer = 0;
            ALuint alSource = 0;
        };

        std::vector<LoadedSource> loaded;
        loaded.reserve(mPendingSources.size());

        for (const auto& src : mPendingSources)
        {
            if (src.filePath.empty()) continue;

            // Decode to mono s16 at the target sample rate.
            ma_decoder_config cfg = ma_decoder_config_init(ma_format_s16, 1, (ma_uint32)sampleRate);
            ma_decoder decoder;
            if (ma_decoder_init_file(src.filePath.c_str(), &cfg, &decoder) != MA_SUCCESS)
            {
                std::cerr << "[ERROR] SpatialEngine: failed to decode " << src.filePath << std::endl;
                continue;
            }

            // Read all PCM frames.
            ma_uint64 totalFrames = 0;
            ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
            if (totalFrames == 0) totalFrames = (ma_uint64)(sampleRate * durationSeconds);

            std::vector<int16_t> pcm((size_t)totalFrames);
            ma_uint64 framesRead = 0;
            ma_decoder_read_pcm_frames(&decoder, pcm.data(), totalFrames, &framesRead);
            ma_decoder_uninit(&decoder);

            if (framesRead == 0) continue;

            // Upload to AL.
            LoadedSource ls;
            alGenBuffers(1, &ls.alBuffer);
            alBufferData(ls.alBuffer, AL_FORMAT_MONO16,
                         pcm.data(),
                         (ALsizei)(framesRead * sizeof(int16_t)),
                         (ALsizei)sampleRate);

            alGenSources(1, &ls.alSource);
            alSourcei(ls.alSource,  AL_BUFFER, (ALint)ls.alBuffer);
            alSource3f(ls.alSource, AL_POSITION, src.position.x, src.position.y, src.position.z);
            alSourcef(ls.alSource,  AL_GAIN, src.volume);
            alSourcei(ls.alSource,  AL_LOOPING, src.looping ? AL_TRUE : AL_FALSE);

            // Attach reverb effect slot if enabled.
            if (mReverbEnabled && mEffectSlot)
            {
                alSource3i(ls.alSource, AL_AUXILIARY_SEND_FILTER,
                           (ALint)mEffectSlot, 0, AL_FILTER_NULL);
            }

            loaded.push_back(ls);
        }

        // ── Step 2: set up MA encoder for WAV output ───────────────────────────
        ma_encoder_config encCfg = ma_encoder_config_init(
            ma_encoding_format_wav, ma_format_s16, 2, (ma_uint32)sampleRate);

        ma_encoder encoder;
        if (ma_encoder_init_file(outputPath.c_str(), &encCfg, &encoder) != MA_SUCCESS)
        {
            std::cerr << "[ERROR] SpatialEngine: failed to open output file: " << outputPath << std::endl;
            for (auto& ls : loaded)
            {
                alDeleteSources(1, &ls.alSource);
                alDeleteBuffers(1, &ls.alBuffer);
            }
            return false;
        }

        // ── Step 3: start all sources ─────────────────────────────────────────
        apply_listener();
        for (auto& ls : loaded)
        {
            alSourcePlay(ls.alSource);
        }

        // ── Step 4: render loop ───────────────────────────────────────────────
        const int kChunkFrames = 1024;
        // Stereo 16-bit: 2 samples per frame.
        std::vector<int16_t> chunk((size_t)(kChunkFrames * 2));

        ma_uint64 totalDurationFrames = (ma_uint64)(durationSeconds * sampleRate);
        ma_uint64 renderedFrames      = 0;

        while (renderedFrames < totalDurationFrames)
        {
            int framesToRender = (int)std::min(
                (ma_uint64)kChunkFrames,
                totalDurationFrames - renderedFrames);

            alcRenderSamplesSOFT(ALDEV, chunk.data(), (ALCsizei)framesToRender);

            ma_uint64 framesWritten = 0;
            ma_encoder_write_pcm_frames(&encoder, chunk.data(),
                                        (ma_uint64)framesToRender, &framesWritten);

            renderedFrames += (ma_uint64)framesToRender;
        }

        // ── Step 5: cleanup ───────────────────────────────────────────────────
        for (auto& ls : loaded)
        {
            alSourceStop(ls.alSource);
            alDeleteSources(1, &ls.alSource);
            alDeleteBuffers(1, &ls.alBuffer);
        }

        ma_encoder_uninit(&encoder);

        std::cout << "[INFO] SpatialEngine: rendered " << durationSeconds
                  << "s to " << outputPath << std::endl;
        return true;
    }

} // namespace naudio
