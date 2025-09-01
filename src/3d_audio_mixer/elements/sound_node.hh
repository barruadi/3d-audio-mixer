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

        public:
            SoundNode():
                mFile(""),
                mPosition(0.0f, 0.0f, 0.0f),
                mVolume(1.0f),
                mPan(0.0f)
            {

            }

            virtual void render();

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
    };
} // namespace nelement
