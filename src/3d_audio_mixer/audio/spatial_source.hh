#pragma once

#include "app/pch.h"
#include <string>

namespace naudio
{
    enum class ReverbPreset
    {
        None,
        SmallRoom,
        LargeHall,
        Cave
    };

    struct SpatialSource
    {
        std::string filePath;
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        float volume       = 1.0f;
        bool looping       = false;
    };
} // namespace naudio
