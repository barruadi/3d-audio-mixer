#pragma once

#include "shader/shader.hh"

namespace nelement
{
    class Element
    {
        public:
            virtual void update() = 0;
    };
} // namespace nelement
