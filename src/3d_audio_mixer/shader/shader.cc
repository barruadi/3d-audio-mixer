#include "shader/shader.hh"
#include "pch.h"

namespace nshader
{
    bool Shader::load()
    {
        mProgramId = glCreateProgram();

        glLinkProgram(mProgramId);
        glValidateProgram(mProgramId);

        return true;
    }

    void Shader::use()
    {
        glUseProgram(mProgramId);
    }

    void Shader::unload()
    {
        glDeleteProgram(mProgramId);
    }
} // namespace nshader
