#pragma once

namespace nshader
{
    class Shader
    {
        private:
            unsigned int mProgramId;

        public:
            Shader() = default;

            bool load();

            void use();

            void unload();
    }
} // namespace nshader
