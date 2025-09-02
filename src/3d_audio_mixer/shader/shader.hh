#pragma once

#include "app/pch.h"

namespace nshader
{
    class Shader
    {
        private:
            unsigned int mProgramId;

        public:
            Shader() = default;

            unsigned int get_compiled_shader(unsigned int shader_type, const std::string& shader_code);

            bool load(const std::string& vertexPath, const std::string& fragmentPath);

            void use();

            void unload();

            void set_mat4(const glm::mat4& mat4, const std::string& name);

            void set_vec3(const glm::vec3& vec3, const std::string& name);
    };
} // namespace nshader
