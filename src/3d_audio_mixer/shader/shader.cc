#include "shader/shader.hh"

namespace nshader
{
    unsigned int Shader::get_compiled_shader(unsigned int shader_type, const std::string& shader_code)
    {
        unsigned int shader = glCreateShader(shader_type);
        const char* code = shader_code.c_str();
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);

        // Check compilation status
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "[SHADER] Shader compilation error: " << infoLog << std::endl;
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }
    
    bool Shader::load(const std::string& vertex_path, const std::string& fragment_path)
    {
        // validate shader file
        std::ifstream vertex_file(vertex_path);
        const std::string vertex_code = std::string((std::istreambuf_iterator<char>(vertex_file)), std::istreambuf_iterator<char>());
        std::ifstream fragment_file(fragment_path);
        const std::string fragment_code = std::string((std::istreambuf_iterator<char>(fragment_file)), std::istreambuf_iterator<char>());

        if (!vertex_file.is_open() || !fragment_file.is_open()) {
            std::cerr << "[SHADER] Failed to open shader files: " << vertex_path << ", " << fragment_path << std::endl;
            return false;
        }

        mProgramId = glCreateProgram();

        unsigned int vertex_shader = get_compiled_shader(GL_VERTEX_SHADER, vertex_code);
        unsigned int fragment_shader = get_compiled_shader(GL_FRAGMENT_SHADER, fragment_code);

        glAttachShader(mProgramId, vertex_shader);
        glAttachShader(mProgramId, fragment_shader);

        glLinkProgram(mProgramId);
        glValidateProgram(mProgramId);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

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

    void Shader::set_mat4(const glm::mat4& mat4, const std::string& name)
	{
		GLint myLoc = glGetUniformLocation(mProgramId, name.c_str());
		glUniformMatrix4fv(myLoc, 1, GL_FALSE, glm::value_ptr(mat4));
	}

    void Shader::set_vec3(const glm::vec3& vec3, const std::string& name)
	{
		GLint myLoc = glGetUniformLocation(mProgramId, name.c_str());
		glUniform3fv(myLoc, 1, glm::value_ptr(vec3));
	}
} // namespace nshader
