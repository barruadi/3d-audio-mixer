#pragma once

#include "shader/shader.hh"

namespace nelement
{
    class Element
    {
        public:
            virtual ~Element() = default;
            virtual void update(nshader::Shader* shader) = 0;
    };
} // namespace nelement
