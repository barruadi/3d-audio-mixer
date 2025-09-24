#pragma once
#include <miniaudio.h>
#include "app/pch.h"

namespace naudio
{
    class AudioContext
    {
        private:
            ma_engine mEngine;

        public:
            bool init();
            void shutdown();

            void set_listener_position(const glm::vec3& position,
                                       const glm::vec3& direction,
                                       const glm::vec3& up);

            ma_engine* get_engine()
            {
                return &mEngine;
            }
    };
} // namespace naudio
