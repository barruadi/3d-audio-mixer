#pragma once

#include "shader/shader.hh"

namespace nelement
{
    class Element
    {
        public:
            virtual void update(nshader::Shader* shader) = 0;
    };
} // namespace nelement
