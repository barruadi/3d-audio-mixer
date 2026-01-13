#pragma once

#include "render/opengl_buffer_manager.hh"
#include "elements/element.hh"

#include "audio/audio_system.hh"
#include "audio/audio_context.hh"

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
            bool mLooping;

            std::shared_ptr<nrender::OpenGL_VertexIndexBuffer> mVertexBuffer;

            // Audio
            naudio::AudioSystem mAudioSystem;
            std::shared_ptr<naudio::AudioContext> mAudioContext;
            int mSoundId;

        public:
            SoundNode()
                : mFile("")
                , mPosition(0.0f, 0.0f, 0.0f)
                , mVolume(1.0f)
                , mPan(0.0f)
                , mLooping(false)
                , mSoundId(-1)
            {
            }

            virtual ~SoundNode() override
            {
                mAudioSystem.shutdown();
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
                // Sync audio position with visual position
                if (mSoundId != -1)
                {
                    mAudioSystem.update(mPosition);
                }
            }

            void self_update(const std::string& file,
                        const glm::vec3& position,
                        float volume,
                        float pan)
            {
                mFile = file;
                set_position(position);
                set_volume(volume);
                mPan = pan;
            }

            void create_buffer()
            {
                std::vector<glm::vec3> positions = {mPosition};
                mVertexBuffer->create_buffers(positions);
            }

            void set_file(const std::string& file)
            {
                // Unload previous sound if exists
                if (mSoundId != -1)
                {
                    mAudioSystem.unload(mSoundId);
                    mSoundId = -1;
                }
                mFile = file;
            }

            void load_sound()
            {
                if (mFile.empty()) return;
                if (!mAudioContext) return;

                // Attach engine if not already
                mAudioSystem.attach_engine(mAudioContext->get_engine());

                // Load returns existing ID if already loaded
                mSoundId = mAudioSystem.load(mFile);
                if (mSoundId != -1)
                {
                    mAudioSystem.set_position(mSoundId, mPosition);
                    mAudioSystem.set_volume(mSoundId, mVolume);
                    mAudioSystem.set_looping(mSoundId, mLooping);
                }
            }

            void play_sound()
            {
                if (mFile.empty()) return;

                // Load if not loaded yet
                if (mSoundId == -1)
                {
                    load_sound();
                }

                if (mSoundId != -1)
                {
                    mAudioSystem.play(mSoundId);
                }
            }

            void stop_sound()
            {
                if (mSoundId != -1)
                {
                    mAudioSystem.stop(mSoundId);
                }
            }

            void pause_sound()
            {
                if (mSoundId != -1)
                {
                    mAudioSystem.pause(mSoundId);
                }
            }

            void resume_sound()
            {
                if (mSoundId != -1)
                {
                    mAudioSystem.resume(mSoundId);
                }
            }

            // Setters
            void set_position(const glm::vec3& position)
            {
                mPosition = position;
                if (mSoundId != -1)
                {
                    mAudioSystem.set_position(mSoundId, mPosition);
                }
                // Update visual buffer
                create_buffer();
            }

            void set_volume(float volume)
            {
                mVolume = volume;
                if (mSoundId != -1)
                {
                    mAudioSystem.set_volume(mSoundId, mVolume);
                }
            }

            void set_looping(bool looping)
            {
                mLooping = looping;
                if (mSoundId != -1)
                {
                    mAudioSystem.set_looping(mSoundId, mLooping);
                }
            }

            void set_engine(const std::shared_ptr<naudio::AudioContext>& audioContext)
            {
                mAudioContext = audioContext;
                if (mAudioContext)
                {
                    mAudioSystem.attach_engine(mAudioContext->get_engine());
                }
            }

            // Getters
            const std::string& get_file_path() const
            {
                return mFile;
            }

            ma_engine* get_engine() const
            {
                if (!mAudioContext) return nullptr;
                return mAudioContext->get_engine();
            }

            glm::vec3 get_position() const
            {
                return mPosition;
            }

            float get_volume() const
            {
                return mVolume;
            }

            bool is_looping() const
            {
                return mLooping;
            }

            bool is_playing() const
            {
                if (mSoundId == -1) return false;
                return mAudioSystem.is_playing(mSoundId);
            }

            bool is_loaded() const
            {
                return mSoundId != -1;
            }

            int get_sound_id() const
            {
                return mSoundId;
            }
    };
} // namespace nelement
